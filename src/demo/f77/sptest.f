C
      DIMENSION ND3(4)
      DATA ND3/32,33,36,40/
C
      CALL GSOPEN
C
      CALL PAGES
         CALL SETFNT(0)
         CALL XCHAR
      CALL PAGEE
C
      CALL PAGES
         CALL SETFNT(2)
         CALL XCHAR
      CALL PAGEE
C
      CALL PAGES
         CALL SETFNT(32)
         CALL XCHAR
      CALL PAGEE
C
      CALL GSCLOS
      STOP
      END
C
      SUBROUTINE XCHAR
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
      DO 600 I=1,13
         ANGL=25*(I-1)
         TILT=25*(I-1)
         CALL SETCHR(0.6,0.4,0.6,ANGL,TILT)
         X=I+6.0
         Y=5.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  600 CONTINUE
C
      DO 700 I=1,13
         ANGL=25*(I-1)
         TILT=-25*(I-1)
         CALL SETCHR(0.6,0.4,0.6,ANGL,TILT)
         X=I+6.0
         Y=3.0
         CALL MOVE(X,Y)
         CALL TEXT('A',1)
  700 CONTINUE
C
         CALL SETCHR(0.6,0.4,0.6,0.0,0.0)
         X=6.0
         Y=1.0
         CALL MOVE(X,Y)
         CALL TEXT('ABCD',4)
         CALL SETCHR(0.6,0.4,0.8,0.0,0.0)
         X=12.0
         Y=1.0
         CALL MOVE(X,Y)
         CALL TEXT('ABCD',4)
         CALL SETCHR(0.6,0.4,1.0,0.0,0.0)
         X=18.0
         Y=1.0
         CALL MOVE(X,Y)
         CALL TEXT('ABCD',4)
      RETURN
      END
