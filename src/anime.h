#ifndef __anime_h__
#define __anime_h__

struct _gsglAnime
{
	gsglboolean Anime;
	FILE *Fp;
	char Tmp[15];
	gsgllist *Frames;
	gsgllist *LastList;
	gsgllist *ListBackUp;
	int Frame_num;
};

#define ANIME(p) (p->Anime)

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_anime_use_anime (gsglPage *);
void gsgl_anime_dont_use_anime (gsglPage *);
gsglboolean gsgl_inq_anime_is_used (gsglPage *);
gsglboolean gsgl_inq_open_tmp_file_for_anime (gsglPage *);
void gsgl_anime_finish_every_page (gsglPage *);
void gsgl_copy_anime_file_to_save_file (gsglPage *);
void gsgl_init_anime_every_page (gsglPage *);
void gsgl_anime_nth_frame (unsigned int n, gsglPage *);
unsigned int gsgl_anime_inq_frame_num (gsglPage *);
void gsgl_anime_replay (gsglPage *);
void gsgl_anime_backup_displaylist (gsglPage *);
gsglboolean gsgl_anime_init_replay (gsglPage *);
int gsgl_inq_current_frame_num (gsglPage *);
void gsgl_set_current_frame_num (int i, gsglPage *);

#ifdef __cplusplus
}
#endif

#endif /* __anime_h__ */
