/* Copyright 2014 Clerk Ma

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.  */

spt_t ng_packet_width (SIGNED_QUAD ch, int ng_font_id)
{
  struct loaded_font * font;
  spt_t width;

  font = &loaded_fonts[ng_font_id];
  width = tfm_get_fw_width(font->tfm_id, ch);
  width = sqxfw(font->size, width);

  return width;
}

extern void ng_set_packet (SIGNED_QUAD ch, int vf_font, SIGNED_QUAD h, SIGNED_QUAD v);

void ng_set (SIGNED_QUAD ch, int ng_font_id, SIGNED_QUAD h, SIGNED_QUAD v)
{
  struct loaded_font * font;
  spt_t width, height, depth;
  unsigned char wbuf[4];

  font = &loaded_fonts[ng_font_id];
  width = tfm_get_fw_width(font->tfm_id, ch);
  width = sqxfw(font->size, width);

  switch (font->type)
  {
    case PHYSICAL:
      if (ch > 65535)
      {
        wbuf[0] = (UTF32toUTF16HS(ch) >> 8) & 0xff;
        wbuf[1] =  UTF32toUTF16HS(ch)       & 0xff;
        wbuf[2] = (UTF32toUTF16LS(ch) >> 8) & 0xff;
        wbuf[3] =  UTF32toUTF16LS(ch)       & 0xff;
        pdf_dev_set_string(h, v, wbuf, 4, width, font->font_id, 2);
      }
      else if (ch > 255)
      {
        wbuf[0] = (ch >> 8) & 0xff;
        wbuf[1] =  ch & 0xff;
        pdf_dev_set_string(h, v, wbuf, 2, width, font->font_id, 2);
      }
      else if (font->subfont_id >= 0)
      {
        unsigned short uch = lookup_sfd_record(font->subfont_id, (unsigned char) ch);
        wbuf[0] = (uch >> 8) & 0xff;
        wbuf[1] =  uch & 0xff;
        pdf_dev_set_string(h, v, wbuf, 2, width, font->font_id, 2);
      }
      else
      {
        wbuf[0] = (unsigned char) ch;
        pdf_dev_set_string(h, v, wbuf, 1, width, font->font_id, 1);
      }

      if (dvi_is_tracking_boxes())
      {
        pdf_rect rect;

        height = tfm_get_fw_height(font->tfm_id, ch);
        depth  = tfm_get_fw_depth (font->tfm_id, ch);
        height = sqxfw(font->size, height);
        depth  = sqxfw(font->size, depth);
        pdf_dev_set_rect(&rect, h, v, width, height, depth);
        pdf_doc_expand_box(&rect);
      }
      break;

    case VIRTUAL:
      ng_set_packet(ch, font->font_id, h, v);
      break;
  }
}
