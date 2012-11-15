#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

template <int XORSTART, int BUFLEN, int XREFKILLER>
class XorStr
{
  private: 
     XorStr();
  public: 
    char s[ BUFLEN ];

     XorStr( const char * xs );
    ~XorStr() { for ( int i = 0; i < BUFLEN; i++ ) s[ i ]=0; }
};

template <int XORSTART, int BUFLEN, int XREFKILLER>
XorStr<XORSTART,BUFLEN,XREFKILLER>::XorStr( const char * xs ) {
  int xvalue = XORSTART;
  int i = 0;

  for ( ; i < ( BUFLEN - 1 ); i++ ) {
    s[ i ] = (char)(xs[ i - XREFKILLER ] ^ xvalue);
    xvalue += 1;
    xvalue %= 256;
  }

  s[ BUFLEN - 1 ] = 0;
}

#endif