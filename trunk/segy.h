#ifndef SEGY_H
#define SEGY_H

#include <QtGui>
#include <segy.h>

class SEGY
{
public:
    double _SampleInterval;
    int    _TotalSamples;
    int    _DataFormat;
    int    _SampleRate;
    int    _TRL;
    int    _TotalTraces;
    int    _CurrentTrace;
    int    _FirstCDP;
    int    _LastCDP;
    double _CDPInc;
    int    _FirstSP;
    int    _LastSP;
    double _SPInc;
    double _FirstX;
    double _FirstY;
    double _LastX;
    double _LastY;
    int    _ByteCDP;
    int    _ByteSP;
    int    _ByteX;
    int    _ByteY;
    int    _ByteLineName;
    int    _ByteInline;
    int    _ByteXline;
    int    _LineNameLength;
    double _EndTime;
    QString _LineName;

    SEGY();
    bool  OpenFile(char*);    
    bool  FileIsOpen();
    void  ReadFirstAndLast();
    void  ShowTraceHeader(long long, QPlainTextEdit*);
    void  ShowBinaryHeader(QPlainTextEdit*);
    void  ShowEbcdicHeader(QPlainTextEdit*);
    void  SetByte(int,int,int,int,int,int,int,int);
    int   Read2Byte(long long,int);
    int   Read4Byte(long long,int);

private: 
    FILE* _F;
    char  _EbcdicHeader [3200];
    char  _BinaryHeader [400];
    char  _TraceHeader  [240];
    char  _INPTRC       [10000];
    float _Data         [10000];
    float GetSample(int);
    bool  ReadTrace(long long);
};

#endif // SEGY_H
