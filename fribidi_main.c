/* FriBidi - Library of BiDi algorithm
 * Copyright (C) 1999 Dov Grobgeld
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*======================================================================
//  A main program for fribidi.
//
//  Dov Grobgeld
//----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>
#include "fribidi.h"

#define CASE(s) if(!strcmp(S_, s))

void die(gchar *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt); 
    
    vfprintf(stderr, fmt, ap);
    exit(-1);
}

#define UNI_ALEF 0x05D0
#define UNI_TAV 0x05EA

/* defining explicit bidi marks */
#define UNI_LRM 0x200E
#define UNI_RLM 0x200F
#define UNI_LRE 0x202a
#define UNI_RLE 0x202b
#define UNI_PDF 0x202c
#define UNI_LRO 0x202d
#define UNI_RLO 0x202e

void charset_to_unicode(gint char_set,
			guchar *s,
			/* output */
			FriBidiChar *us,
			int *length
			)
{
  int i, j;
  int len = strlen(s);

  if (char_set == 0)
    {
      /* Convert A-Z into hebrew characters */
      j = 0;
      for (i=0; i<len+1; i++)
	{
	  guchar ch = s[i];

	  /* we use '_' for explicit marks. The list is:
	   *
	   * _>  LRM
	   * _<  RLM
	   * _l  LRE
	   * _r  RLE
	   * _p  PDF
	   * _L  LRO
	   * _R  RLO
	   * __  underscore itself
	   *
	   */
	  
	  if (ch == '_') {
            (*length)--;
	    switch (ch = s[++i]) {
	      case '>': us[j++] = UNI_LRM; break;
	      case '<': us[j++] = UNI_RLM; break;
	      case 'l': us[j++] = UNI_LRE; break;
	      case 'r': us[j++] = UNI_RLE; break;
	      case 'p': us[j++] = UNI_PDF; break;
	      case 'L': us[j++] = UNI_LRO; break;
	      case 'R': us[j++] = UNI_RLO; break;	    
	      case '_': us[j++] = '_'; break;	    
	      default: i--; break;
	    }	    	  
	  } else if (ch >= 'A' && ch <= 'Z')
	    us[j++] = ch - 'A' + UNI_ALEF;
	  else
	    us[j++] = s[i];
	}
    }
  else if (char_set == FRIBIDI_CHARSET_8859_6)
      fribidi_iso8859_6_to_unicode(s, us);
  else if (char_set == FRIBIDI_CHARSET_8859_8)
      fribidi_iso8859_8_to_unicode(s, us);
  else if (char_set == FRIBIDI_CHARSET_8859_6)
      fribidi_iso8859_6_to_unicode(s, us);
  else if (char_set == FRIBIDI_CHARSET_ISIRI_3342)
      fribidi_isiri_3342_to_unicode(s, us);
  else if (char_set == FRIBIDI_CHARSET_CP1255)
      fribidi_cp1255_to_unicode(s, us);
  else if (char_set == FRIBIDI_CHARSET_CP1256)
      fribidi_cp1256_to_unicode(s, us);
  else if (char_set == FRIBIDI_CHARSET_UTF8)
      *length = fribidi_utf8_to_unicode(s, us);
  else
    die("Sorry! Not implemented!\n");
}

void unicode_to_charset(gint char_set,
			FriBidiChar *us,
			int length,
			/* output */
			gchar *s)
{
  if (char_set == 0)
    {
      /* Convert hebrew characters into A-Z */
      int i, j = 0;
      for (i=0, j=0; i<length; i++) 
	{
	  FriBidiChar ch = us[i];
	  /*{-BE start*/
	  switch (ch)
	    {
	      case UNI_LRM: s[j++] = '_'; s[j++] = '>'; break;
	      case UNI_RLM: s[j++] = '_'; s[j++] = '<'; break;
	      case UNI_LRE: s[j++] = '_'; s[j++] = 'l'; break;
	      case UNI_RLE: s[j++] = '_'; s[j++] = 'r'; break;
	      case UNI_PDF: s[j++] = '_'; s[j++] = 'o'; break;
	      case UNI_LRO: s[j++] = '_'; s[j++] = 'L'; break;
	      case UNI_RLO: s[j++] = '_'; s[j++] = 'R'; break;
	      case '_'    : s[j++] = '_'; s[j++] = '_'; break;
	      default:
	        if (ch >= UNI_ALEF && ch <= UNI_TAV)
	          s[j++] = ch-UNI_ALEF+'A';
	        else if (ch < 256)
	          s[j++] = ch;
	        else
	          s[j++] = '�';
	    }
	}
      s[j] = 0;
    }
  else if (char_set == FRIBIDI_CHARSET_8859_6)
    fribidi_unicode_to_iso8859_6(us, length, s); 
  else if (char_set == FRIBIDI_CHARSET_8859_8)
    fribidi_unicode_to_iso8859_8(us, length, s);
  else if (char_set == FRIBIDI_CHARSET_ISIRI_3342)
    fribidi_unicode_to_isiri_3342(us, length, s);
  else if (char_set == FRIBIDI_CHARSET_CP1255)
    fribidi_unicode_to_cp1255(us, length, s);
  else if (char_set == FRIBIDI_CHARSET_CP1256)
    fribidi_unicode_to_cp1256(us, length, s);
  else if (char_set == FRIBIDI_CHARSET_UTF8)
      (void) fribidi_unicode_to_utf8(us, length, s);
  else
    die("Sorry! Not implemented!\n");
}

int main(int argc, char *argv[])
{
  int argp=1;
  FILE *IN;
  gboolean text_width = 80;
  gboolean do_pad = TRUE;
  gboolean do_fill = FALSE;
  gint char_set = 0;
  guchar *bol_text = NULL, *eol_text = NULL;
  FriBidiCharType input_base_direction = FRIBIDI_TYPE_N;
  

  /* Parse the command line */
  while(argp < argc && argv[argp][0] == '-')
    {
      gchar *S_ = argv[argp++];
      CASE("-help")
	{
	  printf("fribidi - A command line interface to the fribidi library\n"
		 "\n"
		 "Syntax:\n"
		 "    fribidi {options} [filename]\n"
		 "\n"
		 "Options:\n"
		 "    -nopad       Don't right adjustify RTL lines\n"
		 "    -fill        Fill lines up to margin. (Not implemented).\n"
		 "    -width w     Specify width of text\n"
		 "    -eol eol     End lines with the string given by eol.\n"
		 "    -bol bol     Start lines with the string given by bol.\n"
		 "    -rtl         Force base direction to RTL.\n"
		 "    -ltr         Force base direction to LTR.\n"
		 "    -debug       Print debug information.\n"
		 "    -charset cs  Specify charset. Default is CapRTL. Available options are:\n"
		 "                     * 8859-8 (Hebrew)\n"
		 "                     * 8859-6 (Arabic)\n"
		 "                     * CP1255 (Hebrew/Yiddish)\n"
		 "                     * CP1256 (MS-Arabic)\n"
		 "                     * ISIRI-3342 (Farsi)\n"
                 "                     * UTF-8\n"

		 );
	  
	  exit(0);
	}
      CASE("-nopad") { do_pad = FALSE; continue; }
      CASE("-width") { text_width = atoi(argv[argp++]); continue; }
      CASE("-eol")   { eol_text = argv[argp++]; continue; }
      CASE("-bol")   { bol_text = argv[argp++]; continue; }
      CASE("-rtl")   { input_base_direction = FRIBIDI_TYPE_R; continue; }
      CASE("-ltr")   { input_base_direction = FRIBIDI_TYPE_L; continue; }
      CASE("-fill")  { do_fill = TRUE; continue; }
      CASE("-debug") { fribidi_set_debug(1); continue; }
      CASE("-charset")
	{
	  gchar *S_ = argv[argp++];
	  while(1) {
	    CASE("CP1255") { char_set = FRIBIDI_CHARSET_CP1255; break; }
	    CASE("CP1256") { char_set = FRIBIDI_CHARSET_CP1256; break; }
	    CASE("8859-6") { char_set = FRIBIDI_CHARSET_8859_8; break; }
	    CASE("8859-8") { char_set = FRIBIDI_CHARSET_8859_8; break; }
	    CASE("ISIRI-3342") { char_set = FRIBIDI_CHARSET_ISIRI_3342; break;}
            CASE("UTF-8")  { char_set = FRIBIDI_CHARSET_UTF8; break; }
	    die("Unknown char set %s!\n", S_);
	  }
	  continue;
	}
      
	

      die("Unknown option %s!\n", S_);
    }

  /* Open the infile for reading */
  if (argp >= argc)
    {
      IN = stdin;
    }
  else
    {
      gchar *fn = argv[argp++];
      IN = fopen(fn, "r");
    }

  /* Read and process input one line at a time */
  {
    guchar S_[2048];
    
    while(fgets(S_, sizeof(S_), IN))
      {
	int len = strlen(S_);
	FriBidiChar us[2048], out_us[2048];
	guchar outstring[2048];
	FriBidiCharType base;
	int i;

	/* chop */
	if (S_[len-1] == '\n')
	  S_[len-1] = '\0';

	len--;

	charset_to_unicode(char_set, S_, us, &len);
      
	/* Create a bidi string */
	base = input_base_direction;
	fribidi_log2vis(us,
			len,
			&base,
			/* output */
			out_us,
			NULL,   /* No need for log_to_vis mapping */
			NULL,   /* No need for vis_to_log mapping */
			NULL    /* No need for embedding level */
			);

	/* Convert it to something to print */
	unicode_to_charset(char_set, out_us, len, outstring);

	if (bol_text)
	  printf("%s", bol_text);
	
	if (base == FRIBIDI_TYPE_R && do_pad)
	  for (i=0; i<text_width-strlen(outstring); i++) /* debugged ;) */
	    printf(" ");

	printf("%s", outstring);
	
	if (eol_text)
	  printf("%s", eol_text);
	printf("\n");
      } /* one line worth */
  }
  return 0;
}
