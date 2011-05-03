#ifndef _AYEMU_vtxfile_h
#define _AYEMU_vtxfile_h

#include <stdio.h>
#include "ayemu_8912.h"

/* The following constants and data structure comes from
   VTX file format description. You can see them on
   http://
*/

#define AYEMU_VTX_NTSTRING_MAX 255

typedef char NTstring[AYEMU_VTX_NTSTRING_MAX+1];

BEGIN_C_DECLS

/** VTX file format header and status of open file
 * \internal
 *
 * This structure used for save VTX file format header's data
 * (song author, title and so on).
 */
struct VTXFileHeader
{
  ayemu_chip_t chiptype;	/**< Type of sound chip */
  int      stereo;		/**< Type of stereo: 0-ABC, 1-BCA...
			             (see VTX format description) */
  int      loop;		/**< song loop */
  int      chipFreq;		/**< AY chip freq (1773400 for ZX) */
  int      playerFreq;		/**< 50 Hz for ZX, 60 Hz for yamaha */
  int      year;		/**< year song composed */
  NTstring title;		/**< song title  */
  NTstring author;		/**< song author  */
  NTstring from;		/**< song from */
  NTstring tracker;		/**< tracker */
  NTstring comment;		/**< comment */
  size_t   regdata_size;	/**< size of unpacked data (need for unpack). */
};

/**
 * \defgroup vtxfile Functions for extract data from vtx files
 */
/*@{*/


/** structure for VTX file format handler
 * \internal
 * It stores VTX file header and current state
 * (open file pointer, extracted register data, etc).
 */
typedef struct
{
  FILE *fp;			/**< opening .vtx file pointer */
  struct VTXFileHeader hdr;  	/**< VTX header data */
  char *regdata;		/**< unpacked song data */
  size_t frames;		/**< number of AY data frames */
} ayemu_vtx_t;


/** Open vtx file and read vtx file header
    \arg \c vtx - pointer to ayemu_vtx_t structure
    \arg \c filename - filename for open vtx file
    \return Return true if success, else false
*/
EXTERN int ayemu_vtx_open(ayemu_vtx_t *vtx, const char *filename);

/** Read and encode lha data from .vtx file.
 * \return Return pointer to unpacked data or NULL.
 */
EXTERN char *ayemu_vtx_load_data(ayemu_vtx_t *vtx);

/** Print formated file name. If fmt is NULL the default format %a - %t will used
 * \return none.
 */
EXTERN void ayemu_vtx_sprintname(const ayemu_vtx_t *vtx, char *buf,
				 const int sz, const char *fmt);

EXTERN void ayemu_vtx_getframe(const ayemu_vtx_t *vtx, size_t frame_n,
			       unsigned char *regs);

/** Free all of allocaded resource for this file.
 * You must call this function on end work with vtx file
 */
EXTERN void ayemu_vtx_free(ayemu_vtx_t *vtx);

/*@}*/

END_C_DECLS

#endif
