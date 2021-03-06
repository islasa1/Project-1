#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

void readppm(unsigned char *buffer, int *bufferlen, 
             char *header, int *headerlen,
             unsigned *rows, unsigned *cols, unsigned *chans,
             char *file)
{
    char *aline=NULL;  size_t linelen; FILE *filep;
    char magic[2]; unsigned col, row, sat, channels=3;
    int nread=0, toread=0, fd=0;
    *headerlen=0;

    filep=fopen(file, "r");

    // read and validate header
    if((*headerlen += getline(&aline, &linelen, filep)) < 0)
        {perror("getline"); exit(-1);}
    strcat(header, aline);
    sscanf(aline, "%s", magic);
    if(strncmp(magic, "P6", 2) == 0) channels=3; else channels=1;

    // ignore comment line or print for debug
    if((*headerlen += getline(&aline, &linelen, filep)) < 0)
        {perror("getline"); exit(-1);}
    strcat(header, aline);

    if((*headerlen += getline(&aline, &linelen, filep)) < 0)
        {perror("getline"); exit(-1);}
    sscanf(aline, "%u %u", &col, &row);
    strcat(header, aline);
    *bufferlen=row*col*channels;  toread=*bufferlen;
    *rows=row, *cols=col, *chans=channels;

    if((*headerlen += getline(&aline, &linelen, filep)) < 0)
        {perror("getline"); exit(-1);}
    sscanf(aline, "%u", &sat);
    strcat(header, aline);

    printf("%s", header);

    printf("read %d bytes, buffer=%p, toread=%d\n", nread, buffer, toread);

    do
    {
        if((nread=fread(buffer, 1, (col*row*channels), filep)) == 0)
        { 
            if(feof(filep))
                { printf("completed readppm\n"); break; }
            else
                { perror("readppm"); exit(-1); }
        }

        buffer+=nread;
        toread=toread-nread;
        printf("read %d bytes, buffer=%p, toread=%d\n", nread, buffer, toread);
    } while(toread > 0 && (!feof(filep)));

    printf("readppm done\n\n");
    close(filep);
}


void writeppm(unsigned char *buffer, int bufferlen,
              char *header, int headerlen,
              char *file)
{
    FILE *filep;
    int nwritten=0, towrite=0;

    filep=fopen(file, "w");

    towrite=headerlen;
    do
    {
        if((nwritten=fwrite(header, 1, headerlen, filep)) == 0)
        { 
            if(feof(filep))
                { printf("completed writeppm header\n"); break; }
            else
                { perror("writeppm header"); exit(-1); }
        }

        header+=nwritten;
        towrite=towrite-nwritten;
        printf("wrote %d bytes, header=%p, towrite=%d\n", nwritten, header, towrite);
    } while(towrite > 0);
    towrite=0; nwritten=0;

    towrite=bufferlen;
    do
    {
        if((nwritten=fwrite(buffer, 1, bufferlen, filep)) == 0)
        { 
            if(feof(filep))
                { printf("completed writeppm\n"); break; }
            else
                { perror("writeppm"); exit(-1); }
        }

        buffer+=nwritten;
        towrite=towrite-nwritten;
        printf("wrote %d bytes, buffer=%p, towrite=%d\n", nwritten, buffer, towrite);
    } while(towrite > 0);

    close(filep);
}

#define MAX_THREADS (64)
#define ROWS_PER_THREAD (50)

#define PIXIDX ((i*col*chan)+(j*chan)+k)
#define SAT (255)
#define FRAMES_PER_SEC (60)
#define SECONDS (30)


typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[MAX_THREADS];
threadParams_t threadParams[MAX_THREADS];

unsigned char img[640*480*3], newimg[640*480*3];
double alpha=1.25;  unsigned char beta=25;
unsigned row=0, col=0, chan=0;

void *brightenThread(void *threadp)
{
    int sum=0, i, j, k, rowbound;
    unsigned pix;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    rowbound=((threadParams->threadIdx)*ROWS_PER_THREAD)+ROWS_PER_THREAD;

    for(i=(threadParams->threadIdx)*ROWS_PER_THREAD; i < rowbound; i++)
    {
      for(j=0; j < col; j++)
      {
        for(k=0; k < chan; k++)
        {
          newimg[PIXIDX] = (pix=(unsigned)((img[PIXIDX])*alpha)+beta) > SAT ? SAT : pix;
        }
      }
    }

}


void main(int argc, char *argv[])
{
  char header[512];
  int bufflen, hdrlen;
  unsigned pix; 
  int i, j, k, frameidx;

  if(argc < 2) {printf("usage: brighten input.ppm\n"); exit(-1);}

  header[0]='\0';
  readppm(img, &bufflen, header, &hdrlen, &row, &col, &chan, argv[1]);

  for(frameidx=0; frameidx < (FRAMES_PER_SEC * SECONDS); frameidx++)
  {
    for(i=0; i < (row/ROWS_PER_THREAD); i++)
    {
      threadParams[i].threadIdx=i;
      pthread_create(&threads[i],(void *)0,brightenThread,(void *)&(threadParams[i]));
    }

    for(i=0; i < (row/ROWS_PER_THREAD); i++)
       pthread_join(threads[i], NULL);
  }

  writeppm(newimg, bufflen, header, hdrlen, "brighter.ppm");

}
