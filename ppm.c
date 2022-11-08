#include <ppm.h>
#include <stdlib.h>

#ifndef NOT_UNUSED
   #define NOT_UNUSED(x) ((void)(x))
#endif

PPM* PPM_new(const size_t row, const size_t col) {
   PPM* ret = calloc(1, sizeof(*ret) + sizeof(ret->data[0])*row*col);
   if(ret == NULL) {
      perror("Unable to allocate memory for new PPM file!");
      exit(EXIT_FAILURE);
   }
   ret->row = row;
   ret->col = col;
   return ret;
}

PPM* PPM_read(const char* const filename) {
   FILE* fp = fopen(filename, "rb");
   PPM* ret = PPM_readfile(fp);
   fclose(fp);
   return ret;
}

PPM* PPM_readfile(const FILE* const file) {
   NOT_UNUSED(file);
   return NULL;
}

void PPM_write(const PPM* const self, const char* const filename) {
   FILE* fp = fopen(filename, "wb");
   PPM_writefile(self, fp);
   fclose(fp);
}

void PPM_writefile(const PPM* const self, FILE* file) {
   if(file == NULL) {
      return;
   }
   const char* const magic_number = "P6";
   fprintf(file, "%s\n%zu %zu\n255\n", magic_number, self->row, self->col);
   fwrite(self->data, sizeof(self->data[0]), self->row*self->col, file);
}
