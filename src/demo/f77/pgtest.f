C
      DIMENSION ND1(10),ND2(10),ND3(4)
      DATA ND1/0, 2, 3, 4, 5, 6, 7, 8, 9,10/
      DATA ND2/0,11,12,13,14,15,16,17,18,19/
      DATA ND3/0,2,4,6/
C
      CALL GSOPEN
      CALL PAGES
      DO N=0,11
         CALL MOVE(0.5,16.2-N*1.4)
         CALL SETFNT(0)
         CALL SETCHS(0.5,0.0)
         CALL NUMBI(N,'(I2)',2)
         CALL SETFNT(N)
         CALL SETCHS(1.0,0.0)
         CALL MOVE( 2.0,16.1-N*1.4)
         CALl TEXT('ABCDEFGH',8)
         CALL MOVE(12.5,16.1-N*1.4)
         CALl TEXT('abcdefgh',8)
         CALL MOVE(21.0,16.1-N*1.4)
         CALl TEXT('1234',4)
      ENDDO
      CALL PAGEE
C
      CALL PAGES
      CALL SETCHS(0.3,0.0)
      DO N=1,10
         IFNT=ND1(N)
         CALL MOVE(0.5,19.2-N*1.8)
         CALL SETFNT(0)
         CALL NUMBI(IFNT,'(I2)',2)
         CALL SETFNT(IFNT)
         DO I=32,63
            CALL MOVE(2.0+0.5*(I-32),19.2-N*1.8)
            CALl TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(2.0+0.5*(I-64),18.7-N*1.8)
            CALl TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(2.0+0.5*(I-96),18.2-N*1.8)
            CALl TEXT(CHAR(I),1)
         ENDDO
      ENDDO
      CALL PAGEE
C
      CALL PAGES
      CALL SETCHS(0.3,0.0)
      DO N=1,10
         IFNT=ND2(N)
         CALL MOVE(0.5,19.2-N*1.8)
         CALL SETFNT(0)
         CALL NUMBI(IFNT,'(I2)',2)
         CALL SETFNT(IFNT)
         DO I=32,63
            CALL MOVE(2.0+0.5*(I-32),19.2-N*1.8)
            CALl TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(2.0+0.5*(I-64),18.7-N*1.8)
            CALl TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(2.0+0.5*(I-96),18.2-N*1.8)
            CALl TEXT(CHAR(I),1)
         ENDDO
      ENDDO
      CALL PAGEE
C
      CALL PAGES
      Y=18.0
      DO N=1,4
         Y=Y-4.0
         CALL SETFNT(ND3(N))
         CALL SETCHS(0.3,0.0)
         CALL MOVE( 3.0,Y+2.4)
         CALL TEXT('NORMAL$$+SUPER$$=N.',19)
         CALL MOVE(13.0,Y+2.4)
         CALL TEXT('NORMAL$+SUPER$=N.',17)
         CALL MOVE( 3.0,Y+1.8)
         CALL TEXT('NORMAL$$-SUB$$=N.',17)
         CALL MOVE(13.0,Y+1.8)
         CALL TEXT('NORMAL$-SUB$=N.',15)
         CALL MOVE( 3.0,Y+1.2)
         CALL TEXT('NORMAL $$#abc$$# N.',19)
         CALL MOVE(13.0,Y+1.2)
         CALL TEXT('NORMAL $#abc$# N.',17)
         CALL MOVE( 3.0,Y+0.6)
         CALL TEXT('NORMAL $$0abc$$# N.',19)
         CALL MOVE(13.0,Y+0.6)
         CALL TEXT('NORMAL $0abc$# N.',17)
         CALL MOVE( 3.0,Y)
         CALL TEXT('NORMAL$$,A$$;B$$!C$$?DEF$$*BACK.',32)
         CALL MOVE(13.0,Y)
         CALL TEXT('NORMAL$,A$;B$!C$?DEF$*BACK.',27)
      ENDDO
      CALL PAGEE
C
      D=12345.0
      IFNT=0
      CALL PAGES
         CALL INQCHR(CHH,CHW,CHSP,ANGL,TILT)
         CALL SETCHR(CHH,CHW,CHSP,20.0,TILT)
         CALL SETVEW(1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0)
         CALL MOVE( 1.0, 1.0)
         CALL DRAW( 1.0,17.0)
         CALL MOVE(10.0, 1.0)
         CALL DRAW(10.0,17.0)
         CALL MOVE(15.0, 1.0)
         CALL DRAW(15.0,17.0)
         CALL MOVE(20.0, 1.0)
         CALL DRAW(20.0,17.0)
         CALL SETFNT(IFNT)
         DO N=0,3
            CALL GNUMBR( 1.0,16.0-N,D,N,0)
            CALL GNUMBR(10.0,16.0-N,D,N,1)
            CALL GNUMBR(15.0,16.0-N,D,N,2)
            CALL GNUMBR(20.0,16.0-N,D,N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0,11.0-N,D,-N,0)
            CALL GNUMBR(10.0,11.0-N,D,-N,1)
            CALL GNUMBR(15.0,11.0-N,D,-N,2)
            CALL GNUMBR(20.0,11.0-N,D,-N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0, 6.0-N,D,-N-100,0)
            CALL GNUMBR(10.0, 6.0-N,D,-N-100,1)
            CALL GNUMBR(15.0, 6.0-N,D,-N-100,2)
            CALL GNUMBR(20.0, 67.0-N,D,-N-100,3)
         ENDDO
      CALL PAGEE
C
      IFNT=2
      CALL PAGES
      CALL SETVEW(1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0)
         CALL INQCHR(CHH,CHW,CHSP,ANGL,TILT)
         CALL SETCHR(CHH,CHW,CHSP,20.0,-20.0)
         CALL MOVE( 1.0, 1.0)
         CALL DRAW( 1.0,17.0)
         CALL MOVE(10.0, 1.0)
         CALL DRAW(10.0,17.0)
         CALL MOVE(15.0, 1.0)
         CALL DRAW(15.0,17.0)
         CALL MOVE(20.0, 1.0)
         CALL DRAW(20.0,17.0)
         CALL SETFNT(IFNT)
         DO N=0,3
            CALL GNUMBR( 1.0,16.0-N,D,N,0)
            CALL GNUMBR(10.0,16.0-N,D,N,1)
            CALL GNUMBR(15.0,16.0-N,D,N,2)
            CALL GNUMBR(20.0,16.0-N,D,N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0,11.0-N,D,-N,0)
            CALL GNUMBR(10.0,11.0-N,D,-N,1)
            CALL GNUMBR(15.0,11.0-N,D,-N,2)
            CALL GNUMBR(20.0,11.0-N,D,-N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0, 6.0-N,D,-N-100,0)
            CALL GNUMBR(10.0, 6.0-N,D,-N-100,1)
            CALL GNUMBR(15.0, 6.0-N,D,-N-100,2)
            CALL GNUMBR(20.0, 6.0-N,D,-N-100,3)
         ENDDO
      CALL PAGEE
C
      X=4.0
      Y=1.0
      IFNT=2
      IC=0
      DL=1.0
      ANGL=0.0
      TILT=0.0
C
    1 WRITE(6,*) '# INPUT : IFNT,IC,DL,ANGL,TILT ?'
      READ(5,*,ERR=1,END=9000) IFNT,IC,DL,ANGL,TILT
C
      IF(IC.EQ.0) THEN
         CALL PAGES
            CALL MOVE(0.0,17.5)
            CALL TEXT('grfont: IFNT =',14)
            CALL NUMBI(IFNT,'(I5)',5)
            CALL SETCHR(DL,DL/1.5,DL,ANGL,TILT)
            CALL SETFNT(IFNT)
            DO NY=0,7
               DO NX=0,15
                  CALL MOVE(X+NX*1.3,Y+15.0-NY*1.5)
                  CALL TEXT(CHAR(NX+NY*16),1)
               ENDDO
            ENDDO
         CALL PAGEE
      ELSE
         CALL PAGES
            CALL MOVE(0.0,17.5)
            CALL TEXT('grfont: IFNT =',14)
            CALL NUMBI(IFNT,'(I5)',5)
            CALL SETCHR(DL,DL/1.5,DL,ANGL,TILT)
            CALL SETFNT(IFNT)
            CALL MOVE(5.0,5.0)
            CALL TEXT(CHAR(IC),1)
         CALL PAGEE
      ENDIF
C
      GOTO 1
C
 9000 CALL GSCLOS
      END
