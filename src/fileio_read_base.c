#include "gsaf.h"
#include "utils.h"
#include "fileio_read_base.h"

gsglReadFile * gsgl_init_read_file ()
{
	gsglReadFile *r;

	r = (gsglReadFile *) gsgl_malloc (sizeof (gsglReadFile));
	r->surplus = 0;
	r->bytebuffer[0] = 0;
	r->bytebuffer[1] = 0;

	return r;
}

/*-----------------------------------------------------*/
/*     入力されたBase64コードを整数に変換する          */
/*-----------------------------------------------------*/
static unsigned int gsglfread_Base64toInt (unsigned int base64)
{
	if (base64 == 43) /* '+' */
		return 62;
	else if (base64 == 47) /* '/' */
		return 63;
	else if (base64 >= 48 && base64 <= 57) /* '0' ~ '9' */
		return base64 + 4; /* 52 ~ 61 */
	else if (base64 >= 65 && base64 <= 90) /* 'A' ~ 'Z' */
		return base64 - 65; /* 0 ~ 25 */
	else if (base64 >= 97 && base64 <= 122) /* 'a' ~ 'z' */
		return base64 - 71; /* 26 ~ 51 */
	else {
		return 0;
	} 
}

/*************************************************************/
/*             buffer を呼んで token を i4 に返す            */
/*************************************************************/
unsigned char * gsglfread_token (unsigned char * loc, unsigned int *i4, gsglReadFile *r)
{
	unsigned int   c[4];
	unsigned int   integer = 0, tmp, i;
	const static unsigned int   b64[4] = {1, 64, 64*64, 64*64*64};
	const static unsigned int   nByte[3] = {1, 256, 256*256};
	
	r->surplus += 1;

	if (r->surplus == 1) {
		for (i=0; i<4; i++) {
			c[i] = gsglfread_Base64toInt(loc[i]);
			integer += c[i]*b64[i];
		}
		r->bytebuffer[0] = (unsigned int) integer/nByte[2];
		integer -= r->bytebuffer[0]*nByte[2];
		*i4 = integer;
		loc += 4;
	} else if (r->surplus == 2) {
		for (i=0; i<4; i++) {
			c[i] = gsglfread_Base64toInt(loc[i]);
			integer += c[i]*b64[i];
		}
		loc += 4;
		r->bytebuffer[1] = (unsigned int) integer/nByte[2];
		integer -= r->bytebuffer[1]*nByte[2];
		tmp = (unsigned int) integer/nByte[1];
		integer -= tmp*nByte[1];
		*i4 = integer * nByte[1] + r->bytebuffer[0];
		r->bytebuffer[0] = tmp;
	} else if (r->surplus == 3) {
		*i4 = r->bytebuffer[1]*nByte[1]+r->bytebuffer[0];
		r->surplus = 0;
	}
	return loc;
}
