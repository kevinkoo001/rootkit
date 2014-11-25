/**********************************************************
	Last Updated: Nov 18, 2014
	CSE509 System Security 2014 Fall @CS SBU
	Written By: 
		Hyungjoon Koo (hykoo@cs.stonybrook.edu)
		Yaohui Chen (yaohchen@cs.stonybrook.edu)

	Description: 
		Hook the virtual file systems readdir func of the /proc fs, which ps, top user functions 
		will use to get proc information
***********************************************************/

char *(hide_proc[]) = {"1", "2"};


bool init_HJ_proc();
bool rm_HJ_proc();




/*
 *
 *
 * test   ps aux | top 
 *
 *
 *
 */
