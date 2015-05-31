*#RUN :OPT=2 L=JGFYYV0111/GP/GSP L=JGFYYV0111/GP/GDP
      DIMENSION Z(41,41)
      CALL GSOPEN
      DO 1000 I=1,41
      DO 1000 J=1,41
         FX=0.1*FLOAT(I-21)
         FY=0.1*FLOAT(J-21)
         Z(I,J)=-FX**4-FY**4+2.0*(FX**2+FY**2)
 1000 CONTINUE
      IXY=3
      IND=1
      ZMIN= 0.0
      ZMAX= 2.0
      XL=4.0
      YL=4.0
      ZL=2.0
      A=30.0
      B=0.0
      C=-30.0
      D=5.0
      E=25.0
      X1=-3.0
      X2= 3.0
      Y1=-0.5
      Y2= 4.0
C
    1 WRITE(6,*) 'INPUT : IXY,IND,XL,YL,ZL,A,B,C,D,E,',
     &                   'ZMIN,ZMAX,X1,X2,Y1,Y2'
      READ(5,*,END=9999)  IXY,IND,XL,YL,ZL,A,B,C,D,E,
     &                    ZMIN,ZMAX,X1,X2,Y1,Y2
C
      CALL PAGES
      CALL GDEFIN(3.,23.,2.,17.,X1,X2,Y1,Y2)
      CALL GFRAME
      CALL SETLIN(0,0,7)
      CALL PERSE1(Z,41,41,41,ZMIN,ZMAX,IXY,IND,XL,YL,ZL,A,B,C,D,E)
      CALL PAGEE
      GO TO 1
C
 9999 CALL GSCLOS
      STOP
      END

