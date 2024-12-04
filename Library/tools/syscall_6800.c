/*
 *	Generate the syscall functions
 *
 *	6800 via SWI
 *	A holds the syscall number
 *	B holds the argument area size in bytes
 *
 *	On return X holds errno if there is one, D holds the return value
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall_name.h"

static char namebuf[128];

static void write_call(int n)
{
  FILE *fp;
  int i;

  snprintf(namebuf, 128, "fuzix6800/syscall_%s.s",syscall_name[n]);
  fp = fopen(namebuf, "w");
  if (fp == NULL) {
    perror(namebuf);
    exit(1);
  }
  fprintf(fp, "\t.code\n\n");
  fprintf(fp, "\t.export _%s\n\n", syscall_name[n]);
  fprintf(fp, "_%s:\n\tldab #%d\n\tldaa #%d\n", syscall_name[n], 2 * syscall_args[n], n);
  fprintf(fp, "\tswi\n");
  fprintf(fp, "\tcpx @zero\n");
  fprintf(fp, "\tbeq noerror\n");
  fprintf(fp, "\tstx _errno\n");
  fprintf(fp, "noerror:\n");
  /* Non varargs functions do their own tail clean up */
  if (syscall_args[n])
    fprintf(fp, "\tjmp __popret%d\n", syscall_args[n] * 2);
  else
    fprintf(fp, "\trts\n");
  fclose(fp);
}

/* In the varargs case B already holds the argument count info */
static void write_vacall(int n)
{
  FILE *fp;
  snprintf(namebuf, 128, "fuzix6800/syscall_%s.s",syscall_name[n]);
  fp = fopen(namebuf, "w");
  if (fp == NULL) {
    perror(namebuf);
    exit(1);
  }
  fprintf(fp, "\t.code\n\n");
  fprintf(fp, "\t.export _%s\n\n", syscall_name[n]);
  fprintf(fp, "_%s:\n\tldaa #%d\n", syscall_name[n], n);
  /* This works because all varargs syscalls have 2 fixed arguments */
  fprintf(fp, "\taddb #4\n");
  fprintf(fp, "\tswi\n");
  fprintf(fp, "\tcpx @zero\n");
  fprintf(fp, "\tbeq noerror\n");
  fprintf(fp, "\tstx _errno\n");
  fprintf(fp, "noerror:\n");
  fprintf(fp, "\trts\n");
  fclose(fp);
}

static void write_call_table(void)
{
  int i;
  for (i = 0; i < NR_SYSCALL; i++)
    if (syscall_args[i] == VARARGS)
      write_vacall(i);
    else
      write_call(i);
}

static void write_makefile(void)
{
  int i;
  char path[256];
  FILE *fp;

  fp = fopen("fuzix6800/Makefile", "w");
  if (fp == NULL) {
    perror("Makefile");
    exit(1);
  }
  fprintf(fp, "# Autogenerated by tools/syscall6800\n");
  fprintf(fp, "ASRCS = syscall_%s.s\n", syscall_name[0]);
  for (i = 1; i < NR_SYSCALL; i++)
    fprintf(fp, "ASRCS += syscall_%s.s\n", syscall_name[i]);
  fprintf(fp, "\n\nASRCALL = $(ASRCS)\n");
  fprintf(fp, "\nAOBJS = $(ASRCALL:.s=.o)\n\n");
  fprintf(fp, "../syslib.lib: $(AOBJS)\n");
  fprintf(fp, "\techo $(AOBJS) >syslib.l\n");
  fprintf(fp, "\tar rc ../syslib.lib $(AOBJS)\n\n");
  fprintf(fp, "$(AOBJS): %%.o: %%.s\n");
  fprintf(fp, "\tfcc -m6800 -c -o $*.o $<\n\n");
  fprintf(fp, "clean:\n");
  fprintf(fp, "\trm -f $(AOBJS) $(ASRCS) *~\n\n");
  fclose(fp);
}

int main(int argc, char *argv[])
{
  write_makefile();
  write_call_table();
  exit(0);
}
