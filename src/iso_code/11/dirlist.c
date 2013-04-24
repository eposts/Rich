

#include "tdestr.h"
#include "common.h"
#include "define.h"
#include "tdefunc.h"



int  dir_help( WINDOW *window )
{
char dname[MAX_COLS+2]; 
char stem[MAX_COLS+2];  
char drive[_MAX_DRIVE]; 
char dir[_MAX_DIR];     
char fname[_MAX_FNAME]; 
char ext[_MAX_EXT];     
int  rc;
int  file_mode;
int  bin_length;
int  prompt_line;

   if (window != NULL) {
      entab_linebuff( );
      if (un_copy_line( window->ll, window, TRUE ) == ERROR)
         return( ERROR );
      prompt_line = window->bottom_line;
   } else
      prompt_line = g_display.nlines;

   
   dname[0] = '\0';
   rc = get_name( dir1, prompt_line, dname, g_display.message_color );

   if (rc == OK) {
      if (validate_path( dname, stem ) == OK) {
         rc = list_and_pick( dname, stem, window );

        
         if (rc == OK) {
            file_mode = TEXT;
            bin_length = 0;
            _splitpath( dname, drive, dir, fname, ext );
            if (stricmp( ext, ".exe" ) == 0  ||  stricmp( ext, ".com" ) == 0) {
               file_mode = BINARY;
               bin_length = g_status.file_chunk;
            }
            if (window != NULL)
               attempt_edit_display( dname, LOCAL, file_mode, bin_length );
            else
               attempt_edit_display( dname, GLOBAL, file_mode, bin_length );
         }
      } else
         
         error( WARNING,
                window != NULL ? window->bottom_line : g_display.nlines, dir2 );
   }
   return( rc );
}



int  validate_path( char *dname, char *stem )
{
int  rc;
DTA  dta;               
int  fattr;
int  i;
int  len;
char *p;
char temp[MAX_COLS+2];  

   
   if (dname[0] == '\0') {

      assert( strlen( stardotstar ) < MAX_COLS );

      strcpy( dname, stardotstar );
      stem[0] = '\0';
      rc = OK;
   } else {

    
      rc = get_fattr( dname, &fattr );

      if (rc == OK && (fattr & SUBDIRECTORY)) {
         assert( strlen( dname ) < MAX_COLS );
         strcpy( stem, dname );

         
         len = strlen( stem );
         if (stem[len-1] != '\\') {
            strcat( stem, "\\" );
            strcat( dname, "\\" );
         }
         strcat( dname, stardotstar );

      
      } else if (rc != ERROR) {
         if ((rc = my_findfirst( &dta, dname, NORMAL | READ_ONLY | HIDDEN |
                              SYSTEM | SUBDIRECTORY | ARCHIVE )) == OK) {

           

            assert( strlen( dname ) < MAX_COLS );

            strcpy( temp, dname );
            len = strlen( dname );
            for (i=len,p=temp+len; i>=0; i--) {
              
               if (*p == '\\' || *p == ':') {
                  p = temp + i;
                  *(p+1) = '\0';
                  break;
               
               } else if (i == 0) {
                  *p = '\0';
                  break;
               }
               --p;
            }

            assert( strlen( temp ) < MAX_COLS );

            strcpy( stem, temp );
         } else
            rc = ERROR;

      
      } else
         rc = ERROR;
   }
   return( rc );
}



int  list_and_pick( char *dname, char *stem, WINDOW *window )
{
int  rc;
DTA  dta;               
DIRECTORY dir;          
unsigned int cnt;       
FTYPE *flist, *p;       
char cwd[MAX_COLS];     
char dbuff[MAX_COLS];   
char prefix[MAX_COLS];  
int  change_directory = FALSE;
int  stop;
int  len;
int  drive;


   rc = my_findfirst( &dta, dname, NORMAL | READ_ONLY | HIDDEN | SYSTEM |
                                SUBDIRECTORY | ARCHIVE );
   if (rc != ERROR) {
      for (cnt=1; (rc = my_findnext( &dta )) == OK;)
         ++cnt;
      flist = (FTYPE *)calloc( cnt, sizeof(FTYPE) );
   } else
      flist = NULL;
   if (rc != ERROR && flist != NULL) {

      stop = FALSE;
    
      if (stem[1] == ':') {

        
         drive = stem[0];
         if (drive < 'a')
            drive += 32;
         drive = drive - 'a' + 1;
         rc = get_current_directory( dbuff, drive );
         if (rc == ERROR)
            stop = TRUE;
         else {

            
            prefix[0] = (char)(drive - 1 + 'a');
            prefix[1] = ':';
            prefix[2] = '\\';
            prefix[3] = '\0';
            assert( strlen( prefix ) + strlen( dbuff ) < MAX_COLS );
            strcpy( cwd, prefix );
            strcat( cwd, dbuff );
         }

     
      } else {

        
         drive = 0;
         rc = get_current_directory( dbuff, drive );
         if (rc == ERROR)
            stop = TRUE;
         else {

           
            prefix[0] = '\\';
            prefix[1] = '\0';

            assert( strlen( prefix ) + strlen( dbuff ) < MAX_COLS );

            strcpy( cwd, prefix );
            strcat( cwd, dbuff );
         }
      }

      while (stop == FALSE) {
     
         p = flist;
         cnt = 0;

         rc = my_findfirst( &dta, dname, NORMAL | READ_ONLY | HIDDEN | SYSTEM |
                                 SUBDIRECTORY | ARCHIVE );
         if (rc != ERROR) {

        

            assert( strlen( dta.name ) < 14 );

            strcpy( p->fname, dta.name );
            p->fsize = dta.size;
            if (dta.attrib & SUBDIRECTORY)
               strcat( p->fname, "\\" );
            for (cnt=1; (rc = my_findnext( &dta )) == OK; ) {
               ++p;

               assert( strlen( dta.name ) < 14 );

               strcpy( p->fname, dta.name );
               p->fsize = dta.size;
               if (dta.attrib & SUBDIRECTORY)
                  strcat( p->fname, "\\" );
               cnt++;
            }
         }

         if (rc != ERROR) {
            shell_sort( flist, cnt );

            setup_directory_window( &dir, cnt );
            write_directory_list( flist, dir );

          
            rc = select_file( flist, stem, &dir );

            assert( strlen( flist[dir.select].fname ) < MAX_COLS );

            strcpy( dbuff, flist[dir.select].fname );
         }

        
         free( flist );

         if (rc == ERROR)
            stop = TRUE;
         else {
            len = strlen( dbuff );

           
            if (dbuff[len-1] == '\\') {

             

               assert( strlen( stem ) + strlen( dbuff ) < MAX_COLS );

               strcpy( dname, stem );
               strcat( dname, dbuff );
               len = strlen( dname );
               strcpy( dbuff, dname );

            
               dbuff[len-1] = '\0';

               
               rc = set_current_directory( dbuff );
               if (rc == OK) {

                
                  rc = get_current_directory( dbuff, drive );
                  if (rc == OK) {

                     assert( strlen( prefix ) + strlen( dbuff ) < MAX_COLS );

                     strcpy( dname, prefix );
                     strcat( dname, dbuff );
                     change_directory = TRUE;
                  }
               }

              
               if (rc == OK)
                  rc = validate_path( dname, stem );
               if (rc == OK) {
                  rc = my_findfirst( &dta, dname, NORMAL | READ_ONLY | HIDDEN |
                                  SYSTEM | SUBDIRECTORY | ARCHIVE );
                  if (rc != ERROR) {
                     for (cnt=1; (rc = my_findnext( &dta )) == OK;)
                        ++cnt;
                     flist = (FTYPE *)calloc( cnt, sizeof(FTYPE) );
                  }
               }
               if (flist == NULL || rc == ERROR) {
                  stop = TRUE;
                  rc = ERROR;
               }
            } else {

               
               rc = OK;
               stop = TRUE;

               assert( strlen( stem ) + strlen( dbuff ) < MAX_COLS );

               strcpy( dname, stem );
               strcat( dname, dbuff );
            }
         }
      }

     
      if (change_directory)
         set_current_directory( cwd );
      if (window != NULL)
         redraw_screen( window );
   } else {
    
      error( WARNING,  window != NULL ? window->bottom_line : g_display.nlines,
             dir3 );
      rc = ERROR;
   }
   return( rc );
}



void setup_directory_window( DIRECTORY *dir, int cnt )
{
int  i;
int  wid;
char temp[MAX_COLS];    


   dir->col = 3;
   dir->row = 5;
   wid = dir->wid = 72;
   dir->hgt = 16;
   dir->max_cols = 5;
   dir->max_lines = 9;
   dir->cnt = cnt;

  
   dir->lines = dir->cnt / dir->max_cols + (dir->cnt % dir->max_cols ? 1 : 0);
   if (dir->lines > dir->max_lines)
      dir->lines = dir->max_lines;

  
   dir->cols = dir->cnt / dir->lines + (dir->cnt % dir->lines ? 1 : 0);
   if (dir->cols > dir->max_cols)
      dir->cols = dir->max_cols;


   
   dir->avail = dir->lines * dir->cols;

   
   dir->nfiles = dir->cnt > dir->avail ? dir->avail : dir->cnt;

  
   dir->prow = dir->lines - (dir->avail - dir->nfiles);

   
   if (dir->cnt < dir->avail)
      dir->vcols = 0;
   else
      dir->vcols =  (dir->cnt - dir->avail) / dir->max_lines +
                   ((dir->cnt - dir->avail) % dir->max_lines ? 1 : 0);

   
   dir->flist_col[0] = dir->col + 2;
   for (i=1; i<dir->max_cols; i++)
      dir->flist_col[i] = dir->flist_col[i-1] + 14;

   
   for (i=0; i < dir->hgt; i++) {
      if (i == 0 || i == dir->hgt-1) {
         memset( temp, 'Ä', wid );
         temp[wid] = '\0';
         if (i == 0) {
            temp[0] = 'Ú';
            temp[wid-1] = '¿';
         } else {
            temp[0] = 'À';
            temp[wid-1] = 'Ù';
         }
      } else {
         memset( temp, ' ', wid );
         temp[wid] = '\0';
         temp[0] = temp[wid-1] = '³';
      }
      s_output( temp, dir->row+i, dir->col, g_display.help_color );
   }

   
   s_output( dir4, dir->row+1, dir->col+3, g_display.help_color );
   s_output( dir5, dir->row+2, dir->col+3, g_display.help_color );
   s_output( dir6, dir->row+2, dir->col+44, g_display.help_color );
   s_output( dir7, dir->row+14, dir->col+8, g_display.help_color );
}



void write_directory_list( FTYPE *flist, DIRECTORY dir )
{
FTYPE *p, *top;
int  i;
int  j;
int  k;
int  end;
int  line;
int  col;
int  color;

   color = g_display.help_color;
   top = flist;
   for (i=0; i < dir.lines; ++i) {
      p = top;
      end = FALSE;
      for (j=0; j < dir.cols; ++j) {
         col = dir.flist_col[j];
         line = i + dir.row + 4;

         
         s_output( "            ", line, col, color );
         if (!end) {
            s_output( p->fname, line, col, color );
            p += dir.lines;
            k = p - flist;
            if (k >= dir.nfiles)
               end = TRUE;
         }
      }
      ++top;
   }
}



int  select_file( FTYPE *flist, char *stem, DIRECTORY *dir )
{
int  ch;                
int  func;              
int  fno;               
int  goodkey;           
int  r;                 
int  c;                 
int  offset;            
int  stop;              
int  stem_len;          
int  color;             
int  file_color;        
int  change;            
int  oldr;              
int  oldc;              
char asize[20];         
char blank[20];         

   
   memset( blank, ' ', 12 );
   blank[12] = '\0';
   c = r = 1;
   ch = fno = offset = 0;
   color = g_display.help_color;
   file_color = g_display.hilited_file;
   goodkey = TRUE;
   stop = FALSE;
   stem_len = strlen( stem );
   s_output( stem, dir->row+1, dir->col+19, color );
   s_output( flist[fno].fname, dir->row+1, dir->col+19+stem_len, color );
   ltoa( flist[fno].fsize, asize, 10 );
   s_output( blank, dir->row+2, dir->col+19, color );
   s_output( asize, dir->row+2, dir->col+19, color );
   itoa( dir->cnt,  asize, 10 );
   s_output( blank, dir->row+2, dir->col+57, color );
   s_output( asize, dir->row+2, dir->col+57, color );
   xygoto( (c-1)*14+dir->col+2, r+dir->row+3 );
   hlight_line( (c-1)*14+dir->col+2, r+dir->row+3, 12, file_color );
   change = FALSE;
   while (stop == FALSE) {
      oldr = r;
      oldc = c;
      ch = getkey( );
      func = getfunc( ch );

     
      if (ch == RTURN)
         func = Rturn;
      else if (ch == ESC)
         func = AbortCommand;

      switch (func) {
         case Rturn       :
         case NextLine    :
         case BegNextLine :
            stop = TRUE;
            break;
         case AbortCommand :
            stop = TRUE;
            break;
         case LineUp :
            if (r > 1) {
               change = TRUE;
               --r;
            } else {
               r = dir->lines;
               change = TRUE;
               if (offset == 0 || c > 1) {
                  if (c > 1)
                     --c;
               } else if (dir->vcols > 0 && offset > 0 && c == 1) {
                 
                  offset -= dir->lines;
                  recalculate_dir( dir, flist, offset );
               }
            }
            goodkey = TRUE;
            break;
         case LineDown :
            if (r < dir->prow) {
               change = TRUE;
               ++r;
            } else if (r < dir->lines && c != dir->cols) {
               change = TRUE;
               ++r;
            } else {
               change = TRUE;
               r = 1;
               if (offset == dir->vcols * dir->lines || c < dir->cols) {
                  if (c < dir->cols)
                     ++c;
               } else if (dir->vcols > 0 && offset < dir->vcols * dir->lines &&
                         c == dir->cols) {
                  offset += dir->lines;
                  recalculate_dir( dir, flist, offset );
               }
            }
            goodkey = TRUE;
            break;
         case CharLeft :
            if (offset == 0 || c > 1) {
               if (c > 1) {
                  change = TRUE;
                  --c;
               }
            } else if (dir->vcols > 0 && offset > 0 && c == 1) {
               change = TRUE;

               offset -= dir->lines;
               recalculate_dir( dir, flist, offset );
            }
            goodkey = TRUE;
            break;
         case CharRight :
            if (offset == dir->vcols * dir->lines || c < dir->cols) {
               if (c < dir->cols) {
                  change = TRUE;
                  ++c;
                  if (c == dir->cols) {
                     if ( r > dir->prow)
                        r = dir->prow;
                  }
               }
            } else if (dir->vcols > 0 && offset < dir->vcols * dir->lines &&
                         c == dir->cols) {
               change = TRUE;
               offset += dir->lines;
               recalculate_dir( dir, flist, offset );
               if (r > dir->prow)
                  r = dir->prow;
            }
            goodkey = TRUE;
            break;
         case BegOfLine :
            change = TRUE;
            c = r = 1;
            goodkey = TRUE;
            break;
         case EndOfLine :
            change = TRUE;
            r = dir->prow;
            c = dir->cols;
            goodkey = TRUE;
            break;
         case ScreenDown :
            change = TRUE;
            r = (c == dir->cols) ? r = dir->prow : dir->lines;
            goodkey = TRUE;
            break;
         case ScreenUp :
            change = TRUE;
            r = 1;
            goodkey = TRUE;
            break;
         default :
            break;
      }
      if (goodkey) {
         s_output( blank, dir->row+1, dir->col+19+stem_len, color );
         fno = offset + (c-1)*dir->lines + (r-1);
         s_output( flist[fno].fname, dir->row+1, dir->col+19+stem_len, color );
         ltoa( flist[fno].fsize, asize, 10 );
         s_output( blank, dir->row+2, dir->col+19, color );
         s_output( asize, dir->row+2, dir->col+19, color );
         xygoto( (c-1)*14+dir->col+2, r+dir->row+3 );
         goodkey = FALSE;
         if (change) {
            hlight_line( (oldc-1)*14+dir->col+2, oldr+dir->row+3, 12, color );
            hlight_line( (c-1)*14+dir->col+2, r+dir->row+3, 12, file_color );
            change = FALSE;
         }
      }
   }
   dir->select = fno;
   return( func == AbortCommand ? ERROR : OK );
}



void recalculate_dir( DIRECTORY *dir , FTYPE *flist, int offset )
{
register int off;

   off = offset;
   dir->nfiles = (dir->cnt - off) > dir->avail ? dir->avail :
                (dir->cnt - off);
   dir->prow = dir->lines - (dir->avail - dir->nfiles);
   write_directory_list( flist+off, *dir );
}


void shell_sort( FTYPE *flist, int cnt )
{
int  i;
register int j;
register int inc;
FTYPE temp;
FTYPE *fl;

   if (cnt > 1) {
      fl = flist;

     
      j = cnt / 9;
      for (inc=1; inc <= j; inc = 3 * inc + 1);

     
      for (; inc > 0; inc /= 3) {
         for (i=inc; i < cnt; i++) {
            j = i;
            memcpy( &temp, fl+j, sizeof(FTYPE) );
            while (j >= inc && memcmp( fl[j-inc].fname, temp.fname, 14 ) > 0) {
               memcpy( fl+j, fl+j-inc, sizeof(FTYPE) );
               j -= inc;
            }
            memcpy( fl+j, &temp, sizeof(FTYPE) );
         }
      }
   }
}
