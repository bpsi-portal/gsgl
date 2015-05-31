#ifndef __raster_h__
#define __raster_h__

#define RASTER_NON 0
#define RASTER_EPS 1
#define RASTER_PNG 2
#define RASTER_JPG 3

void gsgl2raster_open (char filename[]);
void gsgl2raster_page (int type, int color, int width, int height, void * image_buffer);

#endif
