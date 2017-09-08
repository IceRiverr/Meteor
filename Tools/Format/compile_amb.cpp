#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ansysAmb(const char* iInFileName, const char* iOutFileName){
   FILE* fp1;
   FILE* fp2;
   fp1 = fopen(iInFileName,"rb");
   fp2 = fopen(iOutFileName,"w");
   fseek(fp1, 0L, SEEK_END);  
   int fileSize = ftell(fp1);
   rewind(fp1);
   printf("File size = %d\n",fileSize);
   char* text = new char[fileSize];
   char* iCurr=text;
   memset(text, 0, fileSize);
   fread(text, sizeof(char), fileSize, fp1);
   for(int x=0; (x<=4&&x<fileSize); ++x){
       fprintf(fp2, "%c",*iCurr++);
   }
   int iBones = 0;
   int iDummy = 0;
   int iFrames = 0;
   fprintf(fp2, "\nbones = %d\n",iBones=*(int*)iCurr);
   iCurr+=4;

   fprintf(fp2, "dummy = %d\n",iDummy=*(int*)iCurr);
   iCurr+=4;

   fprintf(fp2, "frames = %d\n",iFrames=*(int*)iCurr);
   iCurr+=4;

   fprintf(fp2, "unknown = %d\n",*(int*)iCurr);
   iCurr+=4;
   for(int i=0; i<iFrames; ++i){
       fprintf(fp2, "frame start flag= %d\n",*(int*)iCurr);
       iCurr+=4;
       fprintf(fp2, "frame index= %d\n",*(int*)iCurr);
       iCurr+=4;
       fprintf(fp2, "vector3df %d = %f, ",i, *(float*)iCurr);
       iCurr+=4;
       fprintf(fp2, "%f, ",*(float*)iCurr);
       iCurr+=4;
       fprintf(fp2, "%f\n",*(float*)iCurr);
       iCurr+=4;
       for(int j=0; j<iBones; ++j){
           fprintf(fp2, "quaternion %d.%d = %f, ", i, j, *(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f, ",*(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f, ",*(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f\n",*(float*)iCurr);
           iCurr+=4;
       }
       for(int k=0; k<iDummy; ++k){
           fprintf(fp2, "Dummy %d name=", k);
           for(int x=0; (x<=4&&x<fileSize); ++x){
               fprintf(fp2, "%c",*iCurr++);
           }
           fprintf(fp2, "\nvector3df %d.%d dummy= %f, ", i, k, *(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f, ",*(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f\n",*(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "quaternion %d.%d dummy = %f, ", i,k, *(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f, ",*(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f, ",*(float*)iCurr);
           iCurr+=4;
           fprintf(fp2, "%f\n",*(float*)iCurr);
           iCurr+=4;
       }
   }
   fclose(fp1);
   fclose(fp2);
   delete[] text;
}
int main(int argc, char** argv){
	ansysAmb("Data/p0.amb", "Data/p0.amb.txt");
    ansysAmb("Data/p3.amb", "Data/p3.amb.txt");
	ansysAmb("Data/character.amb", "Data/character.amb.txt");
	
	printf("Over\n");
    getchar();
    return 0;
}
