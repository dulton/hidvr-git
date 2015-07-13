#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "vo_grid.h"

#define GRID_ALIGN_2BYTE(x) (2 * ((x) / 2))


static unsigned int s_nMaxGrids = 1;

void VO_GRID_init(int maxGrids)
{
	s_nMaxGrids = maxGrids;
}


unsigned int VO_GRID_get_max_row_or_col_by_grids(unsigned int max_grids)
{
	unsigned int s_nMaxGrid = 0;
	int i = 0;
	for(i = 1;; ++i){
		if(max_grids <= i*i){
			s_nMaxGrid = i;
			break;
		}
	}
	return s_nMaxGrid;
}

unsigned int VO_GRID_get_def_divi(unsigned int max_grids)
{
	unsigned int nX = 0, defDivi = VO_DIVISION_X4;
	int i = 0;
	for(i = 1;; ++i){
		if(max_grids <= i*i){
			nX = i;
			break;
		}
	}
	switch (nX){
		case 1:
			defDivi = VO_DIVISION_X1;
			break;
		case 2:
			defDivi = VO_DIVISION_X4;
			break;
		case 3:
			defDivi = VO_DIVISION_X9;
			break;
		case 4:
			defDivi = VO_DIVISION_X16;
			break;
		case 5:
			defDivi = VO_DIVISION_X25;
			break;
		case 6:
			defDivi = VO_DIVISION_X36;
			break;
	}
	return defDivi;
}

unsigned int VO_GRID_get_max_grids(unsigned int max_grids)
{
	unsigned int s_nMaxDiv = 0;
	s_nMaxDiv = pow(VO_GRID_get_max_row_or_col_by_grids(max_grids), 2);
	return s_nMaxDiv;
}

unsigned int VO_GRID_get_max_row_or_col(VO_DIVISION_TYPE divi)
{
	unsigned int _max_divi[] = {1, 1, 2, 3, 4, 3, 4, 4, 5, 6, };
	if (divi < VO_DIVISION_NR)
		return _max_divi[divi];
	else
		return 1;
}


unsigned int VO_GRID_get_grids_of_one_page(VO_DIVISION_TYPE divi)
{
	unsigned int _divi_grids[] = {1, 1, 4, 6, 8, 9, 13, 16, 25, 36, };
	if (divi == VO_DIVISION_DEF) divi = VO_GRID_get_def_divi(s_nMaxGrids);
	if (divi < VO_DIVISION_NR)
		return _divi_grids[divi];
	else
		return 4;
}

unsigned int VO_GRID_grids_to_divi(unsigned int gridsInAPage)
{
	unsigned int _divi_grids[] = {0, 1, 4, 6, 8, 9, 13, 16, 25, 36, };
	int n;
	unsigned int divi = 0;
	for ( n = 0; n < (sizeof(_divi_grids)/sizeof(_divi_grids[0])); n++) {
		if (_divi_grids[n] == gridsInAPage) {
			divi = n;
			break;
		}
	}
	return divi;
}


unsigned int VO_GRID_get_n_pages(VO_DIVISION_TYPE divi)
{
	unsigned int diviGrids = 1;
	if (divi == VO_DIVISION_DEF) divi = VO_GRID_get_def_divi(s_nMaxGrids);
	diviGrids = VO_GRID_get_grids_of_one_page(divi);
	if (diviGrids > 1)
		return ((s_nMaxGrids + diviGrids - 1)/diviGrids);
	else
		return (s_nMaxGrids);
}

unsigned int VO_GRID_get_size_of_grid(VO_DIVISION_TYPE divi, stGRID_RECT *screenRect, stGRID_SIZE *size1, stGRID_SIZE *size2)
{
	unsigned int nRet = 1;
	if (divi >= VO_DIVISION_NR) return 0;
	if (divi == VO_DIVISION_DEF) divi = VO_GRID_get_def_divi(s_nMaxGrids);

	switch(divi) {
		case VO_DIVISION_X1:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height);
			}
			break;
		case VO_DIVISION_X6:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/3);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/3);
			}
			if (size2) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/3) * 2;
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/3) * 2;
			}
			nRet = 2;
			break;
		case VO_DIVISION_X8:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/4);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/4);
			}
			if (size2) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/4) * 3;
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/4) * 3;
			}
			nRet = 2;
			break;
		case VO_DIVISION_X13:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/4);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/4);
			}
			if (size2) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/4) * 2;
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/4) * 2;
			}
			nRet = 2;
			break;
		case VO_DIVISION_X9:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/3);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/3);
			}
			break;
		case VO_DIVISION_X16:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/4);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/4);
			}
			break;
		case VO_DIVISION_X25:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/5);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/5);
			}
			break;
		case VO_DIVISION_X36:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/6);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/6);
			}
			break;
		case VO_DIVISION_X4:
		default:
			if (size1) {
				size1->width = GRID_ALIGN_2BYTE(screenRect->width/2);
				size1->height = GRID_ALIGN_2BYTE(screenRect->height/2);
			}
			break;

	}
	return nRet;
}

/*
* must free rect by usersef
*/
unsigned int VO_GRID_get_rect_of_grid(VO_DIVISION_TYPE divi, stGRID_RECT *screenRect, stGRID_RECT **rect)
{
#define GRID_CALC_VO_CHN_POX_X(chn, sOffX, gridW, divi)  \
	((sOffX) + (gridW) * nPoxXFactor1[divi][chn])
#define GRID_CALC_VO_CHN_POX_Y(chn, sOffY, gridH, divi)  \
	((sOffY) + (gridH) * nPoxYFactor1[divi][chn])
#define GRID_CALC_VO_CHN_W(chn, gridW, divi)  \
	((gridW) * nWFactor1[divi][chn])
#define GRID_CALC_VO_CHN_H(chn, gridH, divi)  \
	((gridH) * nHFactor1[divi][chn])

	unsigned int ii, nRet = 1;
	unsigned int nMaxDivi = 1, gridW = 2, gridH = 2;
	lpGRID_RECT prect = NULL;
	unsigned int nPosXFactor1_X1[] =  {0,}; 
	unsigned int nPosYFactor1_X1[] =  {0,}; 
	unsigned int nWFactor1_X1[] = 	{1,}; 
	unsigned int nHFactor1_X1[] = 	{1,}; 
	unsigned int nPosXFactor1_X4[] =  {0, 1, 0, 1}; 
	unsigned int nPosYFactor1_X4[] =  {0, 0, 1, 1}; 
	unsigned int nWFactor1_X4[] = 	{1, 1, 1, 1}; 
	unsigned int nHFactor1_X4[] = 	{1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X6[] =  {0, 2, 2, 0, 1, 2}; 
	unsigned int nPosYFactor1_X6[] =  {0, 0, 1, 2, 2, 2}; 
	unsigned int nWFactor1_X6[] = 	{2, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X6[] = 	{2, 1, 1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X8[] =  {0, 3, 3, 3, 0, 1, 2, 3}; 
	unsigned int nPosYFactor1_X8[] =  {0, 0, 1, 2, 3, 3, 3, 3}; 
	unsigned int nWFactor1_X8[] = 	{3, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X8[] = 	{3, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X13[] = {1, 0, 1, 2, 3, 0, 3, 0, 3, 0, 1, 2, 3}; 
	unsigned int nPosYFactor1_X13[] = {1, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3}; 
	unsigned int nWFactor1_X13[] = 	{2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X13[] = 	{2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X9[] =  {0, 1, 2, 0, 1, 2, 0, 1, 2}; 
	unsigned int nPosYFactor1_X9[] =  {0, 0, 0, 1, 1, 1, 2, 2, 2}; 
	unsigned int nWFactor1_X9[] =     {1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X9[] =     {1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X16[] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3}; 
	unsigned int nPosYFactor1_X16[] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3}; 
	unsigned int nWFactor1_X16[] =    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X16[] =    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X25[] = {0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4}; 
	unsigned int nPosYFactor1_X25[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4}; 
	unsigned int nWFactor1_X25[] =    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X25[] =    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nPosXFactor1_X36[] = {0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5}; 
	unsigned int nPosYFactor1_X36[] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5}; 
	unsigned int nWFactor1_X36[] =    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	unsigned int nHFactor1_X36[] =    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 

	unsigned int *nPoxXFactor1[] = {
		nPosXFactor1_X4, nPosXFactor1_X1, nPosXFactor1_X4, nPosXFactor1_X6, nPosXFactor1_X8, nPosXFactor1_X9, nPosXFactor1_X13, nPosXFactor1_X16, nPosXFactor1_X25, nPosXFactor1_X36,};
	unsigned int *nPoxYFactor1[] = {
		nPosYFactor1_X4, nPosYFactor1_X1, nPosYFactor1_X4, nPosYFactor1_X6, nPosYFactor1_X8, nPosYFactor1_X9, nPosYFactor1_X13, nPosYFactor1_X16, nPosYFactor1_X25, nPosYFactor1_X36,};
	unsigned int *nWFactor1[] = {
		nWFactor1_X4, nWFactor1_X1, nWFactor1_X4, nWFactor1_X6, nWFactor1_X8, nWFactor1_X9, nWFactor1_X13, nWFactor1_X16, nWFactor1_X25, nWFactor1_X36,};
	unsigned int *nHFactor1[] = {
		nHFactor1_X4, nHFactor1_X1, nHFactor1_X4, nHFactor1_X6, nHFactor1_X8, nHFactor1_X9, nHFactor1_X13, nHFactor1_X16, nHFactor1_X25, nHFactor1_X36,};

	if (divi >= VO_DIVISION_NR)
		divi = VO_DIVISION_DEF;
	if (divi == VO_DIVISION_DEF) divi = VO_GRID_get_def_divi(s_nMaxGrids);

	nMaxDivi = VO_GRID_get_max_row_or_col(divi);
	gridW = GRID_ALIGN_2BYTE(screenRect->width/nMaxDivi);
	gridH = GRID_ALIGN_2BYTE(screenRect->height/nMaxDivi);
	
	nRet = VO_GRID_get_grids_of_one_page(divi);
	if (rect) *rect = (stGRID_RECT *)calloc(nRet, sizeof(stGRID_RECT));
	prect = *rect;
	
	for (ii = 0; ii < nRet; ii++) {
		prect->x = GRID_CALC_VO_CHN_POX_X(ii, screenRect->x, gridW, divi);
		prect->y = GRID_CALC_VO_CHN_POX_Y(ii, screenRect->y, gridH, divi);
		prect->width = GRID_CALC_VO_CHN_W(ii, gridW, divi);
		prect->height = GRID_CALC_VO_CHN_H(ii, gridH, divi);
		prect++;
	}

	return nRet;
}

unsigned int VO_GRID_find_grid_index(VO_DIVISION_TYPE divi, stGRID_RECT *screenRect, int posX, int posY)
{
	lpGRID_RECT prect = NULL;
	unsigned int n , nrect = VO_GRID_get_rect_of_grid(divi, screenRect, &prect);

	if (prect == NULL) return 0;	
	for (n = 0; n < nrect; n++) {
		if (POS_IS_IN_RECT((prect + n), posX, posY) != 0) {
			break;
		}
	}
	free(prect);
	return n;
}

