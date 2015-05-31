C
C     ****** CONT/CONT2/CONT3 TEST PROGRAM ******
C
      CALL GSOPEN
C
    1 WRITE(6,*) '## INPUT : TYPE (0,1,2,3,4) ?'
      READ(5,*,ERR=1,END=9000) ID
C
      IF(ID.EQ.0) THEN
         CALL CTEST0
      ELSEIF(ID.EQ.1) THEN
         CALL CTEST1
      ELSEIF(ID.EQ.2) THEN
         CALL CTEST2
      ELSEIF(ID.EQ.3) THEN
         CALL CTEST3
      ELSEIF(ID.EQ.4) THEN
         CALL CTEST4
      ENDIF
      GOTO 1
C
 9000 CALL GSCLOS
      STOP
      END
C
C     ****** TEST OF LINESP ******
C
      SUBROUTINE CTEST0
C
      DIMENSION X(5),Y(5),XP(20),YP(20)
      DATA X/10.0,15.0,10.0, 5.0,10.0/
      DATA Y/15.0,17.0, 5.0,12.0,15.0/
C
      CALL PAGES
C
      CALL SETLIN(0,0,5)
      CALL LINEPT(X,Y,5,0)
      CALL SETLIN(0,0,6)
      CALL GUSP2D(X,Y,5,XP,YP,20,NP,4)
C      DO 100 I=1,NP
C         WRITE(6,*) XP(I),YP(I)
C  100 CONTINUE
      CALL LINEPT(XP,YP,NP,0)
C
      CALL PAGEE
      RETURN
      END
C
C     ****** TEST OF CONT?1 ******
C
      SUBROUTINE CTEST1
C
      COMMON /CMCONT/ A(41,41),KA(8,41,41) 
      DIMENSION RGB(3,11),ZL(10),RGBL(3,10),ILN(10),WLN(10)
      DATA RGB/0.00,0.00,0.00,
     &         0.20,0.10,0.00,
     &         0.40,0.20,0.00,
     &         0.60,0.30,0.00,
     &         0.80,0.40,0.00,
     &         1.00,0.50,0.00,
     &         1.00,0.60,0.20,
     &         1.00,0.70,0.40,
     &         1.00,0.80,0.60,
     &         1.00,0.90,0.80,
     &         1.00,1.00,1.00/
C
      DO I=1,41
         DO J=1,41
            FX=0.1*FLOAT(I-21)+0.01
            FY=0.1*FLOAT(J-21)
            A(I,J)=FX**4+FY**4-2.0*(FX**2+FY**2)+2.0
         ENDDO
      ENDDO
C
      MODEL=2
      ZORG=0.0
      ZSTEP=0.2
      NSTEP=10
      IPAT=0
      ISPL=0
C
    1 WRITE(6,*) '## INPUT : MODEL(1-5),ZORG,ZSTEP,NSTEP,IPAT,ISPL'
      READ(5,*,ERR=1,END=9999) MODEL,ZORG,ZSTEP,NSTEP,IPAT,ISPL
C
      DO 10 I=1,NSTEP
         ZL(I)=ZSTEP*(I-1)+ZORG
C         ZL(I)=ZSTEP*(I-1)**2/(NSTEP-1)+ZORG
         RGBL(1,I)=0.0
         RGBL(2,I)=0.0
         RGBL(3,I)=0.0
         ILN(I)=IPAT
         WLN(I)=0.0
   10 CONTINUE
C
      CALL PAGES
      CALL GDEFIN(3.,23.,2.,17.,1.,41.,1.,41.)
      CALL GFRAME
      IF(MODEL.EQ.1) THEN
         CALL CONTR1(A,41,41,41,ZORG,ZSTEP,NSTEP,0)
      ELSEIF(MODEL.EQ.2) THEN
C         CALL CONTE1(A,41,41,41,ZL,NSTEP,0,IPAT,KA)
         CALL CONTP1(A,41,41,41,ZORG,ZSTEP,NSTEP,0,IPAT,KA)
      ELSEIF(MODEL.EQ.3) THEN
         CALL CONTQ1(A,41,41,41,ZORG,ZSTEP,NSTEP,0,IPAT,KA)
      ELSEIF(MODEL.EQ.4) THEN
         CALL CONTG1(A,41,41,41,ZL,RGBL,ILN,WLN,NSTEP,ISPL,0,KA)
      ELSEIF(MODEL.EQ.5) THEN
         CALL CONTF1(A,41,41,41,ZL,RGB,NSTEP,0)
         CALL SETLIN(0,0,7)
         CALL GFRAME
         CALL RGBBAR(3.,23.,0.5,1.5,RGB,NSTEP+1,0)
         CALL RGBBAR(23.5,24.5,2.,17.,RGB,NSTEP+1,1)
      ENDIF
      CALL PAGEE
      GO TO 1
C
 9999 RETURN
      END
C
C     ****** TEST OF CONT?2 ******
C
      SUBROUTINE CTEST2
C
      COMMON /CMCONT/ A(41,41),KA(8,41,41) 
      DIMENSION X(41),Y(41)
      DIMENSION RGB(3,11),ZL(10),RGBL(3,10),ILN(10),WLN(10)
      DATA RGB/0.00,0.00,0.00,
     &         0.00,0.10,0.20,
     &         0.00,0.20,0.40,
     &         0.00,0.30,0.60,
     &         0.00,0.40,0.80,
     &         0.00,0.50,1.00,
     &         0.20,0.60,1.00,
     &         0.40,0.70,1.00,
     &         0.60,0.80,1.00,
     &         0.80,0.90,1.00,
     &         1.00,1.00,1.00/
C
      DO 1000 I=1,41
         X(I)=2.0*(0.025*REAL(I-1))**2
 1000 CONTINUE
      DO 1100 J=1,41
         Y(J)=2.0*(0.025*REAL(J-1))**2
 1100 CONTINUE
      DO I=1,41
         DO J=1,41
            A(I,J)=X(I)**4+Y(J)**4-2.0*(X(I)**2+Y(J)**2)+2.0
         ENDDO
      ENDDO
C
      MODEL=2
      ZORG=2.0
      ZSTEP=-0.2
      NSTEP=10
      IPAT=0
      ISPL=0
C
    1 WRITE(6,*) '## INPUT : MODEL(1-5),ZORG,ZSTEP,NSTEP,IPAT,ISPL'
      READ(5,*,ERR=1,END=9999) MODEL,ZORG,ZSTEP,NSTEP,IPAT,ISPL 
C
      DO 10 I=1,NSTEP
         ZL(I)=ZSTEP*(I-1)+ZORG
         RGBL(1,I)=0.0
         RGBL(2,I)=0.0
         RGBL(3,I)=0.0
         ILN(I)=IPAT
         WLN(I)=0.0
   10 CONTINUE
C
      CALL PAGES
      CALL GDEFIN(3.,23.,2.,17.,0.,2.,0.,2.)
      CALL GFRAME
      IF(MODEL.EQ.1) THEN
         CALL CONTR2(A,X,Y,41,41,41,ZORG,ZSTEP,NSTEP,0)
      ELSEIF(MODEL.EQ.2) THEN
         CALL CONTP2(A,X,Y,41,41,41,ZORG,ZSTEP,NSTEP,0,IPAT,KA)
      ELSEIF(MODEL.EQ.3) THEN
         CALL CONTQ2(A,X,Y,41,41,41,ZORG,ZSTEP,NSTEP,0,IPAT,KA)
      ELSEIF(MODEL.EQ.4) THEN
         CALL CONTG2(A,X,Y,41,41,41,ZL,RGBL,ILN,WLN,NSTEP,ISPL,0,KA)
      ELSEIF(MODEL.EQ.5) THEN
         CALL CONTF2(A,X,Y,41,41,41,ZL,RGB,NSTEP,0)
         CALL SETLIN(0,0,7)
         CALL GFRAME
         CALL RGBBAR(3.,23.,0.5,1.5,RGB,NSTEP+1,0)
         CALL RGBBAR(23.5,24.5,2.,17.,RGB,NSTEP+1,1)
      ENDIF
      CALL PAGEE
      GO TO 1
C
 9999 RETURN
      END
C
C     ****** TEST OF CONT?3 ******
C
      SUBROUTINE CTEST3
C
      COMMON /CMCONT/ A(41,41),KA(8,41,41) 
      DIMENSION RGB(3,11),ZL(10),RGBL(3,10),ILN(10),WLN(10),R(41)
      DATA RGB/0.00,0.00,0.00,
     &         0.10,0.20,0.00,
     &         0.20,0.40,0.00,
     &         0.30,0.60,0.00,
     &         0.40,0.80,0.00,
     &         0.50,1.00,0.00,
     &         0.60,1.00,0.20,
     &         0.70,1.00,0.40,
     &         0.80,1.00,0.60,
     &         0.90,1.00,0.80,
     &         1.00,1.00,1.00/
C
      N=41
      PI=2.0*ASIN(1.0)
      DO 1000 I=1,N
         R(I)=REAL(I-1)/REAL(N-1)
 1000 CONTINUE
      DO I=1,N
         DO J=1,N
            RL=R(I)
            TH=2.0*PI*REAL(J-1)/REAL(N)
            A(I,J)=RL**2/(1.0+RL**2)+0.2*SIN(3*TH)
         ENDDO
      ENDDO
C
      MODEL=2
      ZORG=0.0
      ZSTEP=0.1
      NSTEP=10
      IPAT=0
      ISPL=0
C
    1 WRITE(6,*) '## INPUT : MODEL(1-5),ZORG,ZSTEP,NSTEP,IPAT,ISPL'
      READ(5,*,ERR=1,END=9999) MODEL,ZORG,ZSTEP,NSTEP,IPAT,ISPL 
C
      DO 10 I=1,NSTEP
         ZL(I)=ZSTEP*(I-1)+ZORG
         RGBL(1,I)=0.0
         RGBL(2,I)=0.0
         RGBL(3,I)=0.0
         ILN(I)=IPAT
         WLN(I)=0.0
   10 CONTINUE
C
      CALL PAGES
      CALL GDEFIN(5.,20.,2.,17.,-1.0,1.0,-1.0,1.0)
      CALL GFRAME
      IF(MODEL.EQ.1) THEN
         CALL CONTR3(A,R,41,N,N,ZORG,ZSTEP,NSTEP)
      ELSEIF(MODEL.EQ.2) THEN
         CALL CONTP3(A,R,41,N,N,ZORG,ZSTEP,NSTEP,IPAT,KA)
      ELSEIF(MODEL.EQ.3) THEN
         CALL CONTQ3(A,R,41,N,N,ZORG,ZSTEP,NSTEP,IPAT,KA)
      ELSEIF(MODEL.EQ.4) THEN
         CALL CONTG3(A,R,41,N,N,ZL,RGBL,ILN,WLN,NSTEP,ISPL,KA)
         DO 20 I=1,NSTEP
            RGBL(1,I)=1.0
   20    CONTINUE
         CALL CONTG3(A,R,41,N,N,ZL,RGBL,ILN,WLN,NSTEP,2,KA)
      ELSEIF(MODEL.EQ.5) THEN
         CALL CONTF3(A,R,41,N,N,ZL,RGB,NSTEP)
         CALL SETLIN(0,0,7)
         CALL GFRAME
         CALL RGBBAR(5.,20.,0.5,1.5,RGB,NSTEP+1,0)
         CALL RGBBAR(20.5,21.5,2.,17.,RGB,NSTEP+1,1)
      ENDIF
      CALL PAGEE
      GO TO 1
C
 9999 RETURN
      END
C
C     ****** TEST OF CONT?4 ******
C
      SUBROUTINE CTEST4
C
      COMMON /CMCONT/ A(41,41),KA(8,41,41) 
      DIMENSION R(41),T(41)
      DIMENSION RGB(3,11),ZL(10),RGBL(3,10),ILN(10),WLN(10)
      DATA RGB/0.00,0.00,0.00,
     &         0.10,0.00,0.20,
     &         0.20,0.00,0.40,
     &         0.30,0.00,0.60,
     &         0.40,0.00,0.80,
     &         0.50,0.00,1.00,
     &         0.60,0.20,1.00,
     &         0.70,0.40,1.00,
     &         0.80,0.60,1.00,
     &         0.90,0.80,1.00,
     &         1.00,1.00,1.00/
C
      DO 1000 I=1,41
         R(I)=(0.025*REAL(I-1))**2
 1000 CONTINUE
      DO 1100 J=1,41
         T(J)=3.1415926*0.025*REAL(J-1)
 1100 CONTINUE
      DO I=1,41
         DO J=1,41
            A(I,J)=EXP(-3.*R(I)**2)+0.2*COS(4*T(J))*R(I)
         ENDDO
      ENDDO
C
      MODEL=2
      ZORG=1.0
      ZSTEP=-0.1
      NSTEP=10
      IPAT=0
      ISPL=0
C
    1 WRITE(6,*) '## INPUT : MODEL(1-5),ZORG,ZSTEP,NSTEP,IPAT,ISPL'
      READ(5,*,ERR=1,END=9999) MODEL,ZORG,ZSTEP,NSTEP,IPAT,ISPL 
C
      DO 10 I=1,NSTEP
         ZL(I)=ZSTEP*(I-1)+ZORG
         RGBL(1,I)=0.0
         RGBL(2,I)=0.0
         RGBL(3,I)=0.0
         ILN(I)=IPAT
         WLN(I)=0.0
   10 CONTINUE
C
      CALL PAGES
      CALL GDEFIN(3.,23.,3.,13.,-1.,1.,0.,1.)
      CALL GFRAME
      IF(MODEL.EQ.1) THEN
         CALL CONTR4(A,R,T,41,41,41,ZORG,ZSTEP,NSTEP)
      ELSEIF(MODEL.EQ.2) THEN
         CALL CONTP4(A,R,T,41,41,41,ZORG,ZSTEP,NSTEP,IPAT,KA)
      ELSEIF(MODEL.EQ.3) THEN
         CALL CONTQ4(A,R,T,41,41,41,ZORG,ZSTEP,NSTEP,IPAT,KA)
      ELSEIF(MODEL.EQ.4) THEN
         CALL CONTG4(A,R,T,41,41,41,ZL,RGBL,ILN,WLN,NSTEP,ISPL,KA)
      ELSEIF(MODEL.EQ.5) THEN
         CALL CONTF4(A,R,T,41,41,41,ZL,RGB,NSTEP)
         CALL SETLIN(0,0,7)
         CALL GFRAME
         CALL RGBBAR(3.,23.,1.5,2.5,RGB,NSTEP+1,0)
         CALL RGBBAR(23.5,24.5,3.,13.,RGB,NSTEP+1,1)
      ENDIF
      CALL PAGEE
      GO TO 1
C
 9999 RETURN
      END
