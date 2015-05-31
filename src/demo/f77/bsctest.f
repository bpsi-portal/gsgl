C
C     ******************************
C     ****** BSC TEST PROGRAM ******
C     ******************************
C
      IMPLICIT NONE
      CHARACTER KMENU*1
      REAL XMIN, XMAX, YMIN, YMAX
      REAL XCMIN, XCMAX, YCMIN, YCMAX
C
      CALL GSOPEN
C
      XMIN=0.0
      XMAX=25.6
      YMIN=0.0
      YMAX=18.1
      XCMIN=0.0
      XCMAX=25.6
      YCMIN=0.0
      YCMAX=18.1
C
    1 WRITE(6,*) '# INPUT : M/MARK  T/TEXT  L/LINE  S,H,F/CHAR'
      WRITE(6,*) '          V/VIEW  C/CLIP  E/END'
      READ(5,'(A1)',END=9000,ERR=1) KMENU
      CALL GUCPTL(KMENU)
C
      IF(KMENU.EQ.'M') THEN
         CALL PAGES
         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
         CALL XMARK
         CALL PAGEE
      ELSEIF(KMENU.EQ.'T') THEN
         CALL PAGES
         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
         CALL XTEXT
         CALL PAGEE
      ELSEIF(KMENU.EQ.'L') THEN
         CALL PAGES
         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
         CALL XLINE
         CALL PAGEE
      ELSEIF(KMENU.EQ.'H'.OR.KMENU.EQ.'S'.OR.KMENU.EQ.'F') THEN
         CALL PAGES
         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
         CALL XCHAR(KMENU)
         CALL PAGEE
C      ELSEIF(KMENU.EQ.'X') THEN
C         CALL PAGES
C         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
C         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
C         CALL XXYIN
C         CALL PAGEE
      ELSEIF(KMENU.EQ.'V') THEN
   10    WRITE(6,*) '# INPUT : XMIN,XMAX,YMIN,YMAX'
         READ(5,*,END=1,ERR=10) XMIN,XMAX,YMIN,YMAX
         CALL PAGES
         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
         CALL XTEST
         CALL PAGEE
      ELSEIF(KMENU.EQ.'C') THEN
   20    WRITE(6,*) '# INPUT : XCMIN,XCMAX,YCMIN,YCMAX'
         READ(5,*,END=1,ERR=20) XCMIN,XCMAX,YCMIN,YCMAX
         CALL PAGES
         CALL SETVEW(0.0,25.6,0.0,18.1,XMIN,XMAX,YMIN,YMAX)
         CALL SETCLP(XCMIN,XCMAX,YCMIN,YCMAX)
         CALL XCLIP
         CALL PAGEE
C      ELSEIF(KMENU.EQ.'P') THEN
C         XSIZE=25.6
C         YSIZE=18.1
C   30    WRITE(6,*) '# INPUT : XSIZE,YSIZE'
C         READ(5,*,END=1,ERR=30) XSIZE,YSIZE
C         CALL GSSIZE(XSIZE,YSIZE)
C      ELSEIF(KMENU.EQ.'R') THEN
C   40    WRITE(6,*) '# INPUT : FILENAME'
C         READ(5,501,END=1,ERR=40) FLNAME
C  501    FORMAT(A40)
C         OPEN(52,FILE=FLNAME,STATUS='OLD',ERR=40)
C
C         CALL GSLIST(52,NPAGEA,256,NP)
C         WRITE(6,601) (NPAGEA(N),N=1,NP)
C  601    FORMAT(1H ,'## PAGES : ',10(I3:',')/
C     &         (1H ,'           ',10(I3:',')/))
CC
C         XFACT=1.0
C         YFACT=1.0
C         XOFS=0.0
C         YOFS=0.0
C         ROT=0.0
C         ITL=1
C         CALL GSTITL('//')
C   41    WRITE(6,*) '# INPUT : NPAGE,XFACT,YFACT,XOFS,YOFS,ROT,ITL'
C         READ(5,*,END=50,ERR=41) NPAGE,XFACT,YFACT,XOFS,YOFS,ROT,ITL
C         IF(NPAGE.LT.0) THEN
C            CALL PGRLOC(52,-1,IEND)
C         ELSE
C            CALL PGRLOC(52,NPAGE,IEND)
C            IF(IEND.EQ.0) THEN
C               CALL PAGES
C               CALL PGREAD(52,XFACT,YFACT,XOFS,YOFS,ROT,ITL)
C               CALL PAGEE
C            ELSE
C               WRITE(6,*) '## PAGE ',NPAGE,' NOT FOUND'
C            ENDIF
C         ENDIF
C        GOTO 41
C   50    CALL GSTITL('/ /')
C         GOTO 40
      ELSEIF(KMENU.EQ.'E') THEN
         GOTO 9000
      ENDIF
      GOTO 1
C
 9000 CALL GSCLOS
      STOP
      END
C
      SUBROUTINE XTEST
      IMPLICIT NONE
C
      CALL MOVE(1.0,17.1)
      CALL DRAW(1.0,1.0)
      CALL DRAW(24.6,1.0)
      CALL MOVE(5.,5.)
      CALL DRAW(10.,10.)
      CALL SETCHR(1.,1.,2.,0.,0.)
      CALL TEXT('GRAPHIC',7)
      CALL SETCHS(0.30,-90.)
      CALL TEXT('GRAPHIC',7)
      CALL SETCHS(0.30,-180.)
      CALL TEXT('GRAPHIC',7)
C
      RETURN
      END
C
      SUBROUTINE XCLIP
      IMPLICIT NONE
      INTEGER I, N
      REAL X1, Y1, X2, Y2
      REAL GURAND
      EXTERNAL GURAND
C
      N=100
C
      DO 10 I=1,N
         X1=GURAND(25.0)
         Y1=GURAND(20.0)
         CALL MOVE(X1,Y1)
         X2=GURAND(25.0)
         Y2=GURAND(20.0)
         CALL DRAW(X2,Y2)
   10 CONTINUE
      RETURN
      END
C
      SUBROUTINE XMARK
      IMPLICIT NONE
      REAL DEG, R1, R2, ANGL, CA, SA
      INTEGER IDO, I
C
      DEG=3.141593/180.
      R1=1.0
      R2=8.0
      DO 1000 IDO=-31,31
         I=IDO
         ANGL=180.*FLOAT(I)/32.
         CALL SETMRK(I,0.6,0.6,ANGL-90.0,0.0)
         CA=COS(ANGL*DEG)
         SA=SIN(ANGL*DEG)
         CALL MOVE(R1*CA+12.8,R1*SA+9.0)
         CALL MARK(R2*CA+12.8,R2*SA+9.0)
 1000 CONTINUE
      RETURN
      END
C
      SUBROUTINE XTEXT
      IMPLICIT NONE
      REAL X, Y
      INTEGER I, J
C
      CALL SETCHH(0.4,0.0)
      DO I=2,7
         DO J=0,15
            X=J+5.0
            Y=18.0-I
            CALL MOVE(X,Y)
            CALL TEXT(CHAR(16*I+J),1)
         ENDDO
      ENDDO
      CALL SETCHS(0.4,0.0)
      DO I=0,7
         DO J=0,15
            X=J+5.0
            Y=9.0-I
            CALL MOVE(X,Y)
            CALL TEXT(CHAR(16*I+J),1)
         ENDDO
      ENDDO
      RETURN
      END
C
      SUBROUTINE XLINE
      IMPLICIT NONE
      INTEGER ILNDO, ILN
      INTEGER IBLDO, IBL
      INTEGER ICLDO, ICL
      REAL X, Y
C
      DO ILNDO=0,7
         ILN=ILNDO
         DO IBLDO=0,4
            IBL=IBLDO
            DO ICLDO=0,7
               ICL=ICLDO
               X=2*ICL+5
               Y=2*ILN+0.3*IBL+1
               CALL SETLIN(ILN,IBL,ICL)
               CALL MOVE(X,Y)
               CALL DRAW(X+1.5,Y)
            ENDDO
         ENDDO
      ENDDO
      RETURN
      END
C
      SUBROUTINE XCHAR(KID)
      IMPLICIT NONE
      CHARACTER KID*1
      INTEGER I
      REAL ANGL, X, Y, TILT, CHH, CHW
C
      IF(KID.EQ.'S') THEN
         CALL SETFNT(0)
      ELSEIF(KID.EQ.'H') THEN
         CALL SETFNT(0)
      ELSE
         CALL SETFNT(2)
      ENDIF
C
      DO 100 I=1,13
         ANGL=30*(I-1)
         CALL SETCHR(0.6,0.4,0.6,ANGL,0.0)
         X=I+6.0
         Y=15.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  100 CONTINUE
C
      DO 200 I=1,13
         TILT=8*(I-7)
         CALL SETCHR(0.6,0.4,0.6,0.0,TILT)
         X=I+6.0
         Y=13.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  200 CONTINUE
C
      DO 300 I=1,20
         CHH=0.05*(I-1)
         CALL SETCHR(CHH,0.6666667,0.6,0.0,0.0)
         X=I+3.0
         Y=11.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  300 CONTINUE
C
      DO 400 I=1,20
         CHW=0.05*(I-1)
         CALL SETCHR(1.0,CHW*0.6666667,0.6,0.0,0.0)
         X=I+3.0
         Y=9.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  400 CONTINUE
C
      DO 500 I=1,20
         CHH=0.05*(I-1)
         CALL SETCHR(CHH,CHH*0.6666667,0.6,0.0,0.0)
         X=I+3.0
         Y=7.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  500 CONTINUE
C
      CALL SETCHR(0.6,0.4,0.6,0.0,0.0)
      CALL MOVE(3.0,5.0)
      CALL TEXT('NORMAL$$+SUPER$$=N.',19)
      CALL MOVE(14.0,5.0)
      CALL TEXT('NORMAL$+SUPER$=N.',17)
      CALL MOVE(3.0,3.5)
      CALL TEXT('NORMAL$$-SUB$$=N.',17)
      CALL MOVE(14.0,3.5)
      CALL TEXT('NORMAL$-SUB$=N.',15)
      CALL MOVE(3.0,2.0)
      CALL TEXT('NORMAL$$#abc$$#N.',17)
      CALL MOVE(14.0,2.0)
      CALL TEXT('NORMAL$#abc$#N.',15)
      CALL MOVE(3.0,0.5)
      CALL TEXT('NORMAL$$*BACK.',14)
      CALL MOVE(14.0,0.5)
      CALL TEXT('NORMAL$*BACK.',13)
      RETURN
      END
C
C      SUBROUTINE XXYIN
C
C      CHARACTER K*8
C
C         CALL XYIN(X,Y)
C         CALL MOVE(X,Y)
C         CALL SETCHS(0.6,45.)
C         CALL TEXT('GRAPHIC PACKAGE',15)
C         CALL MOVE(X+1.,Y)
C         CALL SETCHS(0.6,0.0)
C         CALL TEXT('INPUT 8 CHAR',12)
C         CALL MOVE(X+1.,Y-1.)
C         CALL CHIN(K,8)
C         CALL SETCHS(0.6,-45.0)
C         CALL TEXT(K,8)
C      RETURN
C      END
