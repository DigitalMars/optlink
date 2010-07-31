#define GINDEX int
#define LINDEX int
#define LARRAY int

GINDEX get_group(LINDEX);
LINDEX next_index(void);
LINDEX install_gindex_lindex(GINDEX,LARRAY *);
int obj_record_bytes_left;
unsigned char next_obj_byte(void);
GINDEX convert_lindex_gindex(LINDEX,LARRAY *);
void put_sm_in_group(GINDEX,GINDEX);

LARRAY group_larray,segmod_larray;


void	grpdef(){
   GINDEX group_gindex,segmod_gindex;

   group_gindex=get_group(next_index());
   install_gindex_lindex(group_gindex,& group_larray);
	while(obj_record_bytes_left > 0){
      if(next_obj_byte() != 0xff) break;
      segmod_gindex=convert_lindex_gindex(next_index(),& segmod_larray);
      put_sm_in_group(group_gindex,segmod_gindex);
   }

}

