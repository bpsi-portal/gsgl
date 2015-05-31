*
*#RUNH:FDS FLTCHK;
*#      L=JGFYYV0111/GP/GSP L=JGFYYV0111/GP/GDP
C
      IMPLICIT REAL * 4  ( A - H , O - Z )
      PARAMETER  ( PI = 3.141596 )
      DIMENSION  X( 21 ) , Y( 21 ) , F( 21 , 21 )
      DIMENSION  P( 21,21 ), Q( 21,21 ), S( 21,21 ), A( 4,21 )
      DIMENSION  C( 21,3 )
      DIMENSION  XG( 101 ) , YG( 101 ) , FG( 101 , 101 ), FF(101)
C
      DO 10  I = 1 , 21
        X( I ) = FLOAT( I - 11 ) * 0.1 * PI
   10 CONTINUE
C
      DO 20  J = 1 , 21
        Y( J ) = FLOAT( J - 11 ) * 0.1 *PI
   20 CONTINUE
C
      DO 30  I = 1 , 21
        DO 40  J = 1 , 21
          F( I , J ) = SIN( 2.*X( I )) * COS( 2.*Y(J)) + 1.
   40   CONTINUE
   30 CONTINUE
      DO 50  I = 1 , 101
        XG( I ) = FLOAT( I - 51 ) * 0.02 *PI
   50 CONTINUE
C
      DO 60  J = 1 , 101
        YG( J ) = FLOAT( J - 51 ) * 0.02 *PI
   60 CONTINUE
C
      CALL GSOPEN
    1 WRITE(6,*) '# INPUT TYPE : 1,2,3 9/end'
      READ(5,*,ERR=1,END=9000) ID
C
      IF(ID.EQ.1) THEN
         CALL TDGRC( F,Y,X,-PI,PI,-PI,PI,0.,2.,21,21,21 )
         CALL SPLN2D( X,Y,F,P,Q,S,A,21,21,21,XG,YG,FG,101,101,101,0 )
         CALL TDGRC( FG,YG,XG,-PI,PI,-PI,PI,0.,2.,101,101,101 )
      ELSEIF(ID.EQ.2) THEN
         CALL TDGR( F,Y,X,-PI,PI,-PI,PI,0.,2.,21,21,21 )
         CALL SPLN2D( X,Y,F,P,Q,S,A,21,21,21,XG,YG,FG,101,101,101,0 )
         CALL TDGR( FG,YG,XG,-PI,PI,-PI,PI,0.,2.,101,101,101 )
      ELSEIF(ID.EQ.3) THEN
         CALL PAGES
         CALL GRAPH1(3.0,23.0,2.0,17.0,X,F(1,11),1,21,1)
         CALL SPLN1D( X,F(1,11),C,21,XG,FF,101,0 )
         CALL GRAPH1(3.0,23.0,2.0,17.0,XG,FF,1,101,1)
         CALL PAGEE
      ELSEIF(ID.EQ.9) THEN
         GOTO 9000
      ENDIF
      GOTO 1
C
 9000 CALL GSCLOS
      STOP
      END
C
C
      SUBROUTINE TDGRC(Z,XD,YD,XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX,NXA,NX,NY)
      DIMENSION Z(NXA,NY),XD(NX),YD(NY),KA(8,101,101)
C
      CALL PAGES
      CALL GDEFIN(3.0,23.0,2.0,17.0,YMIN,YMAX,XMIN,XMAX)
      CALL GFRAME
      CALL CONTQ2(Z,YD,XD,NXA,NY,NX,ZMIN,ZMAX/11,12,0,0,KA)
      CALL PAGEE
C
      RETURN
      END
C
C
      SUBROUTINE TDGR(Z,XD,YD,XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX,NXA,NX,NY)
      DIMENSION Z(NXA,NY),YD(NY),XD(NX),CR(20000),CP(20000)
C
      CALL PAGES
      YC=5./(YMAX-YMIN)
      XC=10./(XMAX-XMIN)
      ZC=10./(ZMAX-ZMIN)
      YS=(YMAX-YMIN)/10.
      XS=(XMAX-XMIN)/10.
      ZS=(ZMAX-ZMIN)/10.
      DO 8000 I=1,20000
         CR(I)=0.0
         CP(I)=0.0
 8000 CONTINUE
C
      CALL MOVE(15.,2.)
      DO 31 IY=1,NY
        X=15.+(YD(IY)-YMIN)*YC-(XD(1)-XMIN)*XC
        Y= 2.+(YD(IY)-YMIN)*YC+(Z(IY,1)-ZMIN)*ZC
        IBC=(X-15.)*NX/(XMAX-XMIN)/XC
        CALL MOVE(X,Y)
        DO 32 IX=2,NX
          X=15.+(YD(IY)-YMIN)*YC-(XD(IX)-XMIN)*XC
          Y= 2.+(YD(IY)-YMIN)*YC+(Z(IY,IX)-ZMIN)*ZC
           IF(Y.GT.CR(9999-IBC+IX)) THEN
              CR(9999-IBC+IX)=Y
             IF((IY-1)/1*1.EQ.IY-1) CALL DRAW(X,Y)
           ELSE
              CALL MOVE(X,Y)
           ENDIF
   32   CONTINUE
   31 CONTINUE
      CALL MOVE(15.,2.)
      DO 41 IX=1,NX
        X=15.+(YD(1)-YMIN)*YC-(XD(IX)-XMIN)*XC
        Y= 2.+(YD(1)-YMIN)*YC+(Z(1,IX)-ZMIN)*ZC
        IBC=(15.-X)*NY/(YMAX-YMIN)/YC
        CALL MOVE(X,Y)
        DO 42 IY=2,NY
          X=15.+(YD(IY)-YMIN)*YC-(XD(IX)-XMIN)*XC
          Y= 2.+(YD(IY)-YMIN)*YC+(Z(IY,IX)-ZMIN)*ZC
           IF(Y.GT.CP(10001+IBC-IY)) THEN
             IF((IX-1)/1*1.EQ.IX-1) CALL DRAW(X,Y)
              CP(10001+IBC-IY)=Y
           ELSE
              CALL MOVE(X,Y)
           ENDIF
   42   CONTINUE
   41 CONTINUE
      CALL MOVE(15.,2.)
      CALL DRAW(21.,8.)
      CALL MOVE(15.,2.)
      CALL DRAW(5.,2.)
      CALL DRAW(5.,12.)
      DO 50 JY=0,10
        X=15.+JY*0.5
        Y= 2.+JY*0.5
        CALL MOVE(X,Y)
        IF(JY/5*5.EQ.JY) THEN
          CALL DRAW(X+0.3,Y)
        ELSE
          CALL DRAW(X+0.15,Y)
        ENDIF
   50 CONTINUE
      DO 60 JX=0,10
        X=15.-JX
        Y= 2.
        CALL MOVE(X,Y)
        IF(JX/5*5.EQ.JX) THEN
          CALL DRAW(X-0.2,Y-0.2)
        ELSE
          CALL DRAW(X-0.1,Y-0.1)
        ENDIF
   60 CONTINUE
      DO 65 JZ=0,10
        X=5.
        Y=2.+JZ
        CALL MOVE(X,Y)
        IF(JZ/5*5.EQ.JZ) THEN
          CALL DRAW(X-0.2,Y)
        ELSE
          CALL DRAW(X-0.1,Y)
        ENDIF
   65 CONTINUE
      IF(ABS(YMAX).GT.1.E-20) THEN
         IOYM=LOG10(ABS(YMAX))
      ELSE
         IOYM=1
      ENDIF
      IF(ABS(YMIN).GT.1.E-20) THEN
         IOYB=LOG10(ABS(YMIN))
      ELSE
         IOYB=1
      ENDIF
      IOY=MAX(IOYM,IOYB)
      DO 70 JY=0,10,5
          RY=YMIN+JY*YS
          X=15.+JY*0.5
          Y= 2.+JY*0.5
          CALL MOVE(X-2.5+0.4*IOY,Y)
          CALL NUMBR(RY,'(F13.2)',13)
   70 CONTINUE
      DO 80 JX=0,10,5
          RX=XMIN+JX*XS
          X=15.-JX
          Y= 1.
          CALL MOVE(X-3.,Y)
          CALL NUMBR(RX,'(F13.2)',13)
   80 CONTINUE
      DO 90 JZ=0,10,5
          RZ=ZMIN+JZ*ZS
          X=0.
          Y=2.+JZ
          CALL MOVE(X,Y)
          CALL NUMBR(RZ,'(F13.2)',13)
   90 CONTINUE
      CALL PAGEE
C
      RETURN
      END
C
      SUBROUTINE GRAPH1(PXMIN,PXMAX,PYMIN,PYMAX,GX,GY,NXM,NXMAX,NGMAX)
C
      DIMENSION GX(NXM),GY(NXM,NGMAX)
C
      CALL GMNMX1(GX,1,NXMAX,1,XMIN,XMAX)
      CALL GMNMX1(GY(1,1),1,NXMAX,1,YMIN,YMAX)
      DO 100 NG=2,NGMAX
         CALL GMNMX1(GY(1,NG),1,NXMAX,1,YMIN1,YMAX1)
         YMIN=MIN(YMIN,YMIN1)
         YMAX=MAX(YMAX,YMAX1)
  100 CONTINUE
      CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,GXSCAL)
      CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,GYSCAL)
C
      IF(GXMAX*GXMAX.GT.0.0) THEN
         GXORG=GXMIN
      ELSE
         GXORG=0.0
      ENDIF
      IF(GYMAX*GYMAX.GT.0.0) THEN
         GYORG=GYMIN
      ELSE
         GYORG=0.0
      ENDIF
C
      CALL SETCHS(0.35,0.0)
      CALL GDEFIN(PXMIN,PXMAX,PYMIN,PYMAX,GXMIN,GXMAX,GYMIN,GYMAX)
      CALL GFRAME
      CALL GSCALE(GXORG,GXSCAL,GYORG,GYSCAL,0.3,9)
      CALL GVALUE(GXORG,2*GXSCAL,GYORG,2*GYSCAL,-2)
      DO 200 NG=1,NGMAX
         CALL SETLIN(-1,-1,7-MOD(NG-1,5))
         CALL GPLOTP(GX,GY(1,NG),1,NXMAX,1,0,0,MOD(NG-1,8))
  200 CONTINUE
      CALL SETLIN(-1,-1,7)
      RETURN
      END

