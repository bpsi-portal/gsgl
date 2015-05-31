#include "gsaf.h"
#include "fileio_base.h"
#include "utils.h"
#include "bsc1.h"
#include "fileio_write.h"
#include "iof.h"

FILE * gsgl_get_fp_write (gsglLibrary *lib)
{
	return lib->wfp;
}

FILE * gsgl_get_fp_read (gsglLibrary *lib)
{
	return lib->rfp;
}

void gsgl_file_read_close (gsglLibrary *lib)
{
	fclose (lib->rfp);
	lib->rfp = NULL;
}

void gsgl_file_write_close (gsglLibrary *lib)
{
	/*printf ("closed\n");*/
	fclose (lib->wfp);
	lib->wfp = NULL;

	gsgl_set_savefile_is_closed ();
}

gsglboolean gsgl_file_write_open (const char *file, gsglLibrary *lib)
{
	if ((lib->wfp = fopen(file, "wb")) == NULL) {
		gsgl_warning ("gsgl_file_write_open", "Could not open file %s", file);
		return GSGL_TRUE;
	}
	/*fprint ("opened\n");*/

	gsgl_set_savefile_is_opened ();

	return GSGL_FALSE;
}

/******************************************************/
/*               Open read file                       */
/******************************************************/ 
void gsgl_file_read_open (char *filename, gsglLibrary *lib)
{
	if ((lib->rfp = fopen((char *)filename, "rb")) == NULL) {
		gsgl_error ("gsgl_file_read_open", "Could not open file %s", filename);
	}
}

/* bellow functions are moved from fio1.f */

/* open graphic file name and save on */
void gsfopn (char *fname, int *ind, gsglLibrary *lib)
{
	gsglboolean lex;
	struct stat  st;

	if (!gsgl_inq_gsopen_is_called ()) return;
	if (gsgl_inq_page_is_open ()) return;

	if (!gsgl_inq_savefile_opened ()) {
		if (stat(fname, &st) != 0) {
			/* dosen't exist */
			lex = GSGL_FALSE;
		} else {
			if (S_ISREG(st.st_mode)) {
				lex = GSGL_TRUE;
			} else {
				gsgl_warning ("gsfopn", "invalid file name");
				*ind = 2;
				return;
			}
		}
		if (!gsgl_file_write_open (fname, lib)) {
			if (lex) {
				fprintf (stdout, " # OLD FILE (%s) IS ASSIGNED.\n", fname);
			} else {
				fprintf (stdout, " # NEW FILE (%s) IS CREATED.\n", fname);
			}
		}
	}
	if (gsgl_inq_savefile_opened ())  {
		*ind = 0;
	} else {
		*ind = 1;
	}
}

/* close graphic file name */
void gsfcls (gsglLibrary *lib)
{
	if (!gsgl_inq_gsopen_is_called ()) return;
	if (gsgl_inq_page_is_open ()) return;

	if (gsgl_inq_save_to_file ()) gsfoff ();

	if (gsgl_inq_savefile_opened ())  {
		gsgl_file_write_close (lib);
		gsgl_set_savefile_is_closed ();
	}
}

/* set graphic file save on */
void gsfon (void)
{
	if (!gsgl_inq_gsopen_is_called ()) return;
	if (gsgl_inq_page_is_open ()) return;

	if (gsgl_inq_savefile_opened ())  {
		gsgl_set_savetofile (GSGL_TRUE);
	} else {
		gsgl_warning ("gsfon", "graphic file is not open");
	}
}

/* set graphic save off */
void gsfoff (void)
{
	if (!gsgl_inq_gsopen_is_called ()) return;
	if (gsgl_inq_page_is_open ()) return;

	gsgl_set_savetofile (GSGL_FALSE);
}

/* set graphic save file */
char * flname (void)
{
	int error;
	char kid;
	gsglboolean lex;
	int len;
	char buffer[256];
	char in[256];
	char *fcsnam = NULL;
	struct stat st;

 START:
	fprintf (stdout, " # INPUT : GRAPHIC SAVE FILE NAME\n");
	fflush (stdout);

	do {
		READ_256_CHAR_F77 (buffer, &error);
		if (error == 2) exit (0);
	} while (error == 1);
	sscanf (buffer, "%s", in);	

	len = strlen (in);
	if ((len >= 5 && strcmp(&(in[len-5]), ".gsgl")) || len < 5) {
		fcsnam = (char *) gsgl_malloc_atomic (sizeof(char) * (len + 5));
		sprintf (fcsnam, "%s.gsgl", in);
	} else {
		fcsnam = (char *) gsgl_malloc_atomic (sizeof(char) * len);
		sprintf (fcsnam, "%s", in);
	}

	if (stat(fcsnam, &st) != 0) {
		/* dosen't exist */
		lex = GSGL_FALSE;
	} else {
		if (S_ISREG(st.st_mode)) {
			lex = GSGL_TRUE;
		} else {
			goto START;
		}
	}

	if (lex && gsgl_inq_ask_in_save ())  {
		do {
                        fprintf (stdout,
                        " # OLD FILE IS GOING TO BE OVERWRITTEN.  ARE YOU SURE (Y) OR (N) ?\n");
			fflush (stdout);

			READ_1_CHAR_F77 (&kid, &error);
			if (error == 2) goto START;
			if (error != 1 && tolower(kid) != 'y') goto START;

		} while (error == 1);
	}

	return fcsnam;
} 

/* save data to file */
void flsave (int npage, gsglPage *Page)
{
	int ibyte;

	if (gsgl_inq_savefile_opened ())  {
		gsgl_save (&ibyte, Page);
		fprintf (stdout, " # PAGE %5d WAS SAVED.  (SAVED BYTES : %10.3f KB)\n",
			 npage, ((float) ibyte)/1000.0);
	}
}
