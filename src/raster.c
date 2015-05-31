#include <png.h>
#include <jpeglib.h>

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef HAVE_STDLIB_H
#undef HAVE_STDLIB_H
#endif

#include "gsaf.h"
#include "raster.h"
#include "utils.h"

static int generateEPS(void * image, int inColor, unsigned int width, unsigned int height);
static int generatePNG(void * image, unsigned int width, unsigned int height);
static int generateJPEG(void * image, unsigned int width, unsigned int height);
static void gsgl2raster_close (void);

int Icolor;
FILE *Out;

/**************************************************************/
/*                  Open graphic file                         */
/**************************************************************/
void gsgl2raster_open (char filename[])
{
	Out = fopen (filename, "wb");  
}

/**************************************************/
/*             Close graphic file                 */
/**************************************************/
void gsgl2raster_page (int type, int color, int width, int height, void * image)
{
	if (type == RASTER_EPS) 
		generateEPS (image, color, width, height);
	if (type == RASTER_PNG)
		generatePNG (image, width, height);
	if (type == RASTER_JPG)
		generateJPEG (image, width, height);
	
	gsgl2raster_close ();
} 

/***************************************************/
/*            Create a graphic file                */
/***************************************************/
static void gsgl2raster_close (void)
{
	fclose (Out);
	Out = NULL;
} 

static int generateEPS (void * image_buffer, int icolor, unsigned int width, unsigned int height)
{
	unsigned char *curpix;
	int components, pos, i;
	
	curpix = (unsigned char *) image_buffer;
	if (curpix == NULL)
		return 1;
	if (icolor)
		components = 3;     /* Red, green, blue. */
	else
		components = 1;     /* Luminance. */
	
	fprintf(Out, "%%!PS-Adobe-2.0 EPSF-1.2\n");
	fprintf(Out, "%%%%Creator: GSAF/OpenGL pixmap render output\n");
	fprintf(Out, "%%%%BoundingBox: 0 0 %d %d\n", width, height);
	fprintf(Out, "%%%%EndComments\n");
	fprintf(Out, "gsave\n");
	fprintf(Out, "/bwproc {\n");
	fprintf(Out, "    rgbproc\n");
	fprintf(Out, "    dup length 3 idiv string 0 3 0\n");
	fprintf(Out, "    5 -1 roll {\n");
	fprintf(Out, "    add 2 1 roll 1 sub dup 0 eq\n");
	fprintf(Out, "    { pop 3 idiv 3 -1 roll dup 4 -1 roll dup\n");
	fprintf(Out, "        3 1 roll 5 -1 roll put 1 add 3 0 }\n");
	fprintf(Out, "    { 2 1 roll } ifelse\n");
	fprintf(Out, "    } forall\n");
	fprintf(Out, "    pop pop pop\n");
	fprintf(Out, "} def\n");
	fprintf(Out, "systemdict /colorimage known not {\n");
	fprintf(Out, "    /colorimage {\n");
	fprintf(Out, "        pop\n");
	fprintf(Out, "        pop\n");
	fprintf(Out, "        /rgbproc exch def\n");
	fprintf(Out, "        { bwproc } image\n");
	fprintf(Out, "    } def\n");
	fprintf(Out, "} if\n");
	fprintf(Out, "/picstr %d string def\n", width * components);
	fprintf(Out, "%d %d scale\n", width, height);
	fprintf(Out, "%d %d %d\n", width, height, 8);
	fprintf(Out, "[%d 0 0 %d 0 0]\n", width, height);
	fprintf(Out, "{currentfile picstr readhexstring pop}\n");
	fprintf(Out, "false %d\n", components);
	fprintf(Out, "colorimage\n");
	
	pos = 0;
	/*for (i = width * height * (components); i > 0; i--) {*/
	for (i = width * height * (components+1); i > 0; i--) {
		if (i%4 != 1)
			fprintf(Out, "%02x", *curpix);
		curpix++;
		if (++pos >= 48) {
			fprintf(Out, "\n");
			pos = 0;
		}
	}
	if (pos)
		fprintf(Out, "\n");
	
	fprintf(Out, "grestore\n");
	fprintf(Out, "showpage\n");
	return 0;
}

static int generatePNG (void * image_buffer, unsigned int width, unsigned int height)
{
	unsigned char **image, *p;
	int i, j;
	png_structp     png_ptr;
	png_infop       info_ptr;

	p = (unsigned char *) image_buffer;

	image = (png_bytepp) gsgl_malloc (height * sizeof(png_bytep));
	if (image == NULL) exit(1);
	for (j = 0; j < height; j++) {
		/*image[j] = (png_bytep) gsgl_malloc (width * sizeof(png_byte) * 3);*/
		image[j] = (png_bytep) gsgl_malloc_atomic (width * sizeof(png_byte) * 4); 
		if ( image[j] == NULL ) exit(1);
	}
	for (j = height; j > 0; j--) {
		/*for (i = 0; i < width * 3; i++) {*/
		for (i = 0; i < width * 4; i++) {
			image[j-1][i] = (unsigned char) *p;
			p++;
		}
	}
	
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, Out);
	png_set_IHDR(png_ptr, info_ptr, width, height,
		     8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, image);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	
	gsgl_free (image);
	return 0;
}

static int generateJPEG (void * image_buffer, unsigned int width, unsigned int height)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	unsigned char *p;
	unsigned char *line;
	int i, j, k;
	int quality = 100;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	jpeg_stdio_dest(&cinfo, Out);

	cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);
  
	p = (unsigned char *) image_buffer;
	p += width * 4 * (height -1);

	line = gsgl_malloc_atomic (4 * width * sizeof(unsigned char));
	
	for (i = 0; i < height; i++) {
		k = 0;
		for (j = 0; j < 4 * width; j++) {
			if ((j+1) % 4 != 0) {
				line [k] = p[j];
				k++;
			}
		}
		row_pointer[0] = line;
		(void) jpeg_write_scanlines (&cinfo, row_pointer, 1);
		p -= width * 4;
	}
	
	gsgl_free (line);
	jpeg_finish_compress (&cinfo);
	jpeg_destroy_compress (&cinfo);

	return 0;
}
