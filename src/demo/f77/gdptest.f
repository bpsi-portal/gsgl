C
C     ****** TEST PROGRAM FOR GSAF BSC2 ******
C
      CHARACTER KID*1
C
      CALL GSOPEN
C
    1 WRITE(6,*) "SELECT : C/CLIP  M/COLOR  Q/QUIT"
      READ(5,'(A1)',ERR=1,END=9000) KID
      CALL GUCPTL(KID)
C
      IF(KID.EQ.'C') THEN
         CALL XCLIP
      ELSEIF(KID.EQ.'M') THEN
         CALL XCOLOR
      ELSEIF(KID.EQ.'Q') THEN
         GOTO 9000
      ENDIF
      GOTO 1
C
 9000 CALL GSCLOS
      STOP
      END
C
C     ****** TEST GENERAL AND CLIPING ******
C
      SUBROUTINE XCLIP
C
      DIMENSION X0(100),Y0(100),X(100),Y(100)
C
      X1=8.0
      X2=18.0
      Y1=7.0
      Y2=12.0
    1 WRITE(6,*) ' INPUT : X1,X2,Y1,Y2'
      READ(5,*,ERR=1,END=9000) X1,X2,Y1,Y2
      CALL PAGES
         CALL SETCLP(X1,X2,Y1,Y2)
         DO 100 I=1,5
            X0(I)=5*SIN(0.5*(I-1))+9.5
            Y0(I)=5*COS(0.5*(I-1))+8
  100    CONTINUE
         X0(6)=X0(1)
         Y0(6)=Y0(1)
C
         DO 200 K=1,6
            DO 150 I=1,6
               X(I)=X0(I)-1.0*K
               Y(I)=Y0(I)
  150       CONTINUE
            CALL SETLNW(0.03*K)
            CALL LINES(X,Y,6)
  200    CONTINUE
C
         CALL SETLNW(0.0)
         DO 400 K=1,6
            DO 350 I=1,6
               X(I)=X0(I)+1.0*(K+1)
               Y(I)=Y0(I)
  350       CONTINUE
            CALL SETRGB(1.0,0.16*K,1.0)
            CALL POLY(X,Y,6)
  400    CONTINUE
      CALL PAGEE
      GOTO 1
C
 9000 CONTINUE
      RETURN
      END
C
C     ***** TEST MANY COLORS *****
C
      SUBROUTINE XCOLOR
C
      DIMENSION XA(5),YA(5)
C
      NCOLOR = 27
C
    1 WRITE(6,*) 'HOW MANY COLORS ?'
      READ(5,*,ERR=1,END=9000) NCOLOR
      IF (NCOLOR.EQ.0) GOTO 9000
C
      NC1=INT(REAL(NCOLOR)**(1.0/3.0))
      NCT=INT(REAL(NCOLOR)/REAL(NC1))
      NC2=INT(SQRT(REAL(NCT)))
      NC3=INT(REAL(NCT)/REAL(NC2))
C
      WRITE(6,*) 'NCOLOR = ',NC1,' * ',NC2,' * ',NC3,' = ',NC1*NC2*NC3
C
      W=0.3
      DX1=23.0/NC1
      DY2=15.0/NC2
      DX3=(DX1-W)/NC3
      DX=DX3
      DY=DY2-W
C
      CALL PAGES
C
      DO 1000 N1=1,NC1
         X0=1.0+DX1*(N1-1)
         R=(N1-1)/REAL(NC1-1)
      DO 1000 N2=1,NC2
         Y=16.0-DY2*(N2-1)
         G=(N2-1)/REAL(NC2-1)
      DO 1000 N3=1,NC3
         X=X0+DX3*(N3-1)
         B=(N3-1)/REAL(NC3-1)
         XA(1)=X
         YA(1)=Y
         XA(2)=X+DX
         YA(2)=Y
         XA(3)=X+DX
         YA(3)=Y-DY
         XA(4)=X
         YA(4)=Y-DY
         XA(5)=X
         YA(5)=Y
         CALL SETRGB(R,G,B)
         CALL POLY(XA,YA,5)
 1000 CONTINUE
      CALL PAGEE
      GOTO 1
C
 9000 RETURN
      END
