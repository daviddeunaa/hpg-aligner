#include "statistics.h"

//---------------------------------------------------------------------------------------------------------------------

statistics_t* statistics_new(char** section_labels_p, char** section_sublabels_p, unsigned int *num_values, int num_sections, int num_subsections) {
  statistics_t* s_p = (statistics_t*)calloc(1, sizeof(statistics_t));
  
  pthread_mutex_init(&(s_p->mutex), NULL);
  
  s_p->num_sections = num_sections;
  s_p->num_subsections = num_subsections;
  s_p->section_labels_p = (char**) calloc(num_sections, sizeof(char*));
  s_p->section_sublabels_p = (char**) calloc(num_subsections, sizeof(char*));
  s_p->values_p   = (size_t**) calloc(num_sections, sizeof(size_t*));
  s_p->num_values = (unsigned int *)calloc(num_sections, sizeof(unsigned int));
  int i;

  for (i = 0; i < num_sections; i++) {
    s_p->section_labels_p[i] = strdup(section_labels_p[i]);
    s_p->values_p[i] = (size_t*)calloc(num_values[i], sizeof(size_t));
    s_p->num_values[i] = num_values[i];
  }

  for ( i = 0; i < num_subsections; i++) {
     s_p->section_sublabels_p[i] = strdup(section_sublabels_p[i]);
  }
  return s_p;
} 

//----------------------------------------------------------------------------------------------------------------------

void statistics_free(statistics_t* statistics_p) {
    for (int i = 0; i < statistics_p->num_sections; i++) {
      if (statistics_p->section_labels_p[i] != NULL) { free(statistics_p->section_labels_p[i]); } 
    }
    for (int i = 0; i < statistics_p->num_subsections; i++) {
      if (statistics_p->section_sublabels_p[i] != NULL) { free(statistics_p->section_sublabels_p[i]); } 
    }
}

//----------------------------------------------------------------------------------------------------------------------

void statistics_set(unsigned int section, unsigned int subsection, size_t value, statistics_t* statistics_p) {
  statistics_p->values_p[section][subsection] = value;
}

//----------------------------------------------------------------------------------------------------------------------

void statistics_add(unsigned int section, unsigned int subsection, size_t value, statistics_t* statistics_p) {
  pthread_mutex_lock(&statistics_p->mutex);
  statistics_p->values_p[section][subsection] += value;
  pthread_mutex_unlock(&statistics_p->mutex);
}

//---------------------------------------------------------------------------------------------------------------------

void statistics_display(statistics_t* statistics_p) {
  if (statistics_p == NULL) return;

  printf("\n");
  printf("===========================================================\n");
  printf("=                  S t a t i s t i c s                    =\n");
  printf("===========================================================\n");
  int i, j, s, start = 0, end = 0;
  for (i = 0; i < statistics_p->num_sections; i++) {
      printf("-----------------------------------------------------------\n");
      printf("%s\n", statistics_p->section_labels_p[i]);
      printf("-----------------------------------------------------------\n");
      end += statistics_p->num_values[i];
      s = 0;
      for (j = start; j < end; j++) {
	printf("\t%s:%-7lu\n", statistics_p->section_sublabels_p[j], statistics_p->values_p[i][s]);
	s++;
      }
      //printf("\n");
      start = j;
  }
  
  printf("===========================================================\n");
  printf("===========================================================\n");
  printf("\n");
}

//-------------------------------------------------------------------------------------------------------------------

void timing_and_statistics_display(statistics_t* statistics_p, timing_t *t_p) {

  if (t_p == NULL) return;

  printf("\n");
  printf("=============================================================================\n");
  printf("=                  O P E R A T I O N S   N U M B E R                        =\n");
  printf("=============================================================================\n");
  int i, j;

  for (i = 3 ; i < t_p->num_sections - 3; i++) {
    t_p->section_times_p[i] = 0;
    for (j = 0; j < t_p->num_threads_p[i]; j++) {
      if (t_p->section_times_p[i] < t_p->thread_times_p[i][j]) {
	t_p->section_times_p[i] = t_p->thread_times_p[i][j];
      }
    }

    if ( i ==  (t_p->num_sections - 4)) {
      printf("%s\t: %lu/%4.04f = %4.04f(op/sec)\n", t_p->section_labels_p[i], statistics_p->values_p[i - 2][4], t_p->section_times_p[i]/1000000, 
	     (statistics_p->values_p[i - 2][4] / (t_p->section_times_p[i]/1000000)) );
    } else {
      printf("%s\t: %lu/%4.04f = %4.04f(op/sec)\n", t_p->section_labels_p[i], statistics_p->values_p[i- 2][1], t_p->section_times_p[i]/1000000, 
	     (statistics_p->values_p[i][1] / (t_p->section_times_p[i]/1000000)) );
    }
  }
  printf("=============================================================================\n");
  printf("=============================================================================\n");
  printf("\n");

}

