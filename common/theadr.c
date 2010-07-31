void theadr(void)
{
   i=nextbyte();
   theadr_temp_len=i;
   movebytes(i,theadr_temp);

   if (!found_theadr) {
      found_theadr=TRUE;
      if (in_library) {
         strip_path_ext();
      }
   }
   curn_theadr=NULL;
}


