/*
 *  CPUTimer.h
 *
 *  Created by Humberto Longo on 02/04/13.
 *  Instituto de Informatica - UFG
 *
 */

#ifndef CPUTIME_H
 #define CPUTIME_H

#include <iostream>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

class CPUTimer
{
  public:
    CPUTimer();

    // Retorna o tempo (em segs e msegs) de CPU cronometrado para uma rotina.
    // Se apenas uma cronometragem foi realizada, entao os valores retornados
    // por getCPUCurrSecs() e getCPUTtotalSecs sao iguais.
    double getCPUCurrSecs();

    // Retorna o tempo total (em segs e msegs) de CPU cronometrado para uma rotina
    double getCPUTotalSecs();

    // Retorna o tempo (em segs e msegs) de execucao cronometrado para uma rotina.
    // Se apenas uma cronometragem foi realizada, entao os valores retornados
    // por getCPUCurrSecs() e getCPUTtotalSecs sao iguais.
    double getCronoCurrSecs();

    // Retorna o tempo total (em segs e msegs) de execucao cronometrado para uma rotina.
    double getCronoTotalSecs();

    // Inicia a cronometragem (tempo de execucao e de CPU) de uma rotina
    bool start();

    // Encerra a cronometragem (tempo de execucao e de CPU) de uma rotina
    bool stop();

    // Prepara o ambiente de cronometragem para ser utilizado em outra rotina
    void reset();

    // Operator to add cputimers
    void operator += ( CPUTimer t );

    inline void increaseCPUTotalSecs( double s )
    {
      CPUTotalSecs += s;
    };

  private:
    double CPUTStart;   // the cpu start time
    double CPUTStop;    // the cpu stop time
    double CronoTStart; // the real start time
    double CronoTStop;  // the real stop time

    double getCPUTime();
    double getRealTime();

    // tempo de cpu cronometrado para uma rotina (segs e msegs)
    double CPUCurrSecs;

    // total do tempo de cpu cronometrado para uma rotina (segs e msegs)
    double CPUTotalSecs;

    // tempo de execucao cronometrado para uma rotina (segs e msegs)
    double CronoCurrSecs;

    // total do tempo de execucao cronometrado para uma rotina (segs e msegs)
    double CronoTotalSecs;

    bool gottime; // do we have a measured time we can return?
    bool started; // is the timer started?
};
#endif
