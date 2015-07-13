#ifndef __VO_GRID_H__
#define __VO_GRID_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GRID_SIZE
{
	unsigned int width;
	unsigned int height;
}stGRID_SIZE, *lpGRID_SIZE;
#define GRID_SET_SIZE(P,W,H) (P)->width = (W); (P)->height = (H)

typedef struct GRID_RECT
{
	int x, y;
	unsigned int width, height;
}stGRID_RECT, *lpGRID_RECT;
#define GRID_SET_RECT(P,X,Y,W,H) (P)->width = (W); (P)->height = (H); (P)->x = (X); (P)->y = (Y)


typedef enum {
	VO_DIVISION_DEF = 0, // max div
	VO_DIVISION_X1,
	VO_DIVISION_X4,
	VO_DIVISION_X6,
	VO_DIVISION_X8,
	VO_DIVISION_X9,
	VO_DIVISION_X13,
	VO_DIVISION_X16,
	VO_DIVISION_X25,
	VO_DIVISION_X36,
	
	VO_DIVISION_NR
}VO_DIVISION_TYPE;


void VO_GRID_init(int maxGrids);

unsigned int VO_GRID_get_max_row_or_col_by_grids(unsigned int max_grids);
unsigned int VO_GRID_get_def_divi(unsigned int max_grids);
unsigned int VO_GRID_get_max_grids(unsigned int max_grids);
unsigned int VO_GRID_get_max_row_or_col(VO_DIVISION_TYPE divi);
unsigned int VO_GRID_get_grids_of_one_page(VO_DIVISION_TYPE divi);
unsigned int VO_GRID_grids_to_divi(unsigned int gridsInAPage);
unsigned int VO_GRID_get_n_pages(VO_DIVISION_TYPE divi);
unsigned int VO_GRID_get_size_of_grid(VO_DIVISION_TYPE divi, stGRID_RECT *screenRect, 
	stGRID_SIZE *size1, stGRID_SIZE *size2);
unsigned int VO_GRID_get_rect_of_grid(VO_DIVISION_TYPE divi, stGRID_RECT *screenRect, stGRID_RECT **rect);
unsigned int VO_GRID_find_grid_index(VO_DIVISION_TYPE divi, stGRID_RECT *screenRect, int posX, int posY);
	
#define POS_IS_IN_RECT(rect, nx, ny) ( ((nx) >= rect->x) && ((nx) < (rect->x + rect->width)) && ((ny) >= rect->y) && ((ny) < (rect->y + rect->height)))
#define VO_GRID_CHN_IS_IN_PAGE(div,page,ch) ((ch>=page*VO_GRID_get_grids_of_one_page(div)) && (ch<(page+1)*VO_GRID_get_grids_of_one_page(div)))

#define VO_GRID_HAS_BIG_GRID(divi) (((divi) == VO_DIVISION_X1) || ((divi) == VO_DIVISION_X6) || ((divi) == VO_DIVISION_X8) || ((divi) == VO_DIVISION_X13))
#define VO_GRID_BASE_GRID_OF_PAGE(divi, page) ((page) * VO_GRID_get_grids_of_one_page(divi))
#define VO_GRID_END_GRID_OF_PAGE(divi, page) (((page) + 1) * VO_GRID_get_grids_of_one_page(divi))

#ifdef __cplusplus
}
#endif
#endif


