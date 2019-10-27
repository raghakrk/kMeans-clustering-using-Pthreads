/*
 * kmeanpthread.cpp
 *
 *  Created on: Sep 27, 2019
 *      Author: raghak
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<iostream>
#include <pthread.h>
#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"
#define cluster_size 6
using namespace std;
float means[6] = { 0,65,100,125,190,255};
int clustval[6]={0,51,102,153,204,255};
int NUM_THREADS;
unsigned char* a = (unsigned char*)malloc(sizeof(unsigned char) * h * w);
unsigned char* b = (unsigned char*)calloc(h * w,sizeof(unsigned char));

pthread_mutex_t lock;
pthread_cond_t cv;
int r;
struct  threaddata{
   int	thread_id;
   float sumv[cluster_size]={0,0,0,0,0,0};
   int no[cluster_size]={1,1,1,1,1,1};
};

void *kmean(void *threadarg)
{

	//cout<<"A:"<<double(a[260488])<<endl;
	struct threaddata *data;
	data=(struct threaddata *) threadarg;
	int id=data->thread_id;
	int val; int index;
	int minv;
	float dist;
	int iterations;
	for (iterations<0;iterations<50;iterations++)
	{
		for (int i = id*((800*800)/NUM_THREADS); i < (id+1)*((800*800)/NUM_THREADS); i++)
		{
			//cout<<i<<endl;
			minv = INT8_MAX;
			index=-1;
			val=double(a[i]);
			for (int j = 0; j < 6; j++)
			{
				dist = abs((val- means[j]));
				if (dist < minv)
				{
					minv = dist;
					index = j;
				}
			}
			data->sumv[index]+=a[i];
			data->no[index]+=1;
		}
		pthread_mutex_lock(&lock);
		if (r<NUM_THREADS-1)
		{
			r++;
			pthread_cond_wait(&cv,&lock);

		}
		if(r==NUM_THREADS-1){
			for (int k=0;k<6;k++)
			{
				means[k]=data->sumv[k]/data->no[k];
			}
			r=0;
			pthread_cond_broadcast(&cv);
		}
		pthread_mutex_unlock(&lock);

	}
	pthread_exit(NULL);
}


int main(int argc, char** argv) {
	NUM_THREADS=atoi(argv[1]);
	//NUM_THREADS=1;
	struct timespec start, stop;
  	double time;
	FILE* fp;
	pthread_mutex_init(&lock,NULL);
	pthread_cond_init(&cv,NULL);

	// the matrix is stored in a linear array in row major fashion
	if (!(fp = fopen(input_file, "rb"))) {
		cout << "can not opern file" << endl;
		return 1;
	}
	fread(a, sizeof(unsigned char), w * h, fp);
	fclose(fp);
	//cout<<double(a[260488])<<endl;

	// measure the start time here
	if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}

	//  Your code goes here
	int rc;
	void *status;
	pthread_t threads[NUM_THREADS];
	struct threaddata thread_data_array[NUM_THREADS];
	for( int ii = 0; ii< NUM_THREADS; ii++ ) {
	   thread_data_array[ii].thread_id = ii;
	   rc = pthread_create(&threads[ii], NULL, kmean, (void *) &thread_data_array[ii]);
	  if (rc) {
		 cout << "Error:unable to create thread," << rc << endl;
		 exit(-1);
	  }
	}
	for (int j = 0; j < NUM_THREADS; j++)
	{
	   rc=pthread_join (threads [j], &status);
	   if (rc) {
				 printf("ERROR; return code from pthread_join() is %d\n", rc);
				 exit(-1);
				 }
	}
	for (int i=0;i<800*800;i++)
	{
		float dist;
		int minv = INT8_MAX;
		int index=-1;
		for (int j=0;j<6;j++)
		{	dist = abs((a[i]- means[j]));
					if (dist < minv)
					{
						minv = dist;
						index = j;
					}
		}
		b[i]=clustval[index];
	}

	// measure the end time here
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror( "clock gettime" );}
	  time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

	// print out the execution time here
	cout<<"Number of FLOPs = " <<2*h*w<<", Execution time = "<<time<<" sec,"<<2*h*w/time/1e6<<" MFLOPs per sec"<<endl;

	if (!(fp = fopen(output_file, "wb"))) {
	cout << "can not opern file" << endl;
	return 1;
	}
	fwrite(b, sizeof(unsigned char), w * h, fp);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cv);
	fclose(fp);
	return 0;
}
