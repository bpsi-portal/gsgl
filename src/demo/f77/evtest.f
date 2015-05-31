C
C     ******************************
C     ****** EV TEST PROGRAM ******
C     ******************************
C
      DIMENSION XR(4),YR(4)
      CHARACTER KIN*1
C
      CALL GSOPEN
C
      CALL PAGES
C         CALL DVINFO(I1,I2,I3,I4,I5)
C         WRITE(6,*) '# cells,planes,depth,white,black =',
C     &                 I1,I2,I3,I4,I5
      XMIN=1.0+GURAND(22.6)
      YMIN=1.0+GURAND(14.0)
      XMAX=XMIN+2.0
      YMAX=YMIN+2.0
      CALL MOVE(XMIN,YMIN)
      CALL DRAW(XMAX,YMIN)
      CALL DRAW(XMAX,YMAX)
      CALL DRAW(XMIN,YMAX)
      CALL DRAW(XMIN,YMIN)
      WRITE(6,'(4F10.4)') XMIN,XMAX,YMIN,YMAX
C     
      CALL GF_SET_EVENT(15)
    1    CONTINUE
            CALL GF_GET_EVENT(ID,X,Y,KID,KEY)
            CALL ASCCHR(KID,KIN,1)
            WRITE(6,'(I6,2F10.4,2I6,4X,A1)') ID,X,Y,KEY,KID,KIN
            CALL GUCPTL(KIN)
            IF(ID.EQ.-1) GOTO 10
            IF(ID.EQ.1.AND.KIN.EQ.'Q') GOTO 10
            IF(ID.EQ.4.AND.
     &           (X.GT.XMIN).AND.
     &           (X.LT.XMAX).AND.
     &           (Y.GT.YMIN).AND.
     &           (Y.LT.YMAX)) GOTO 10
         GOTO 1
C
   10    CONTINUE
      CALL GF_SET_EVENT(0)
      CALL PAGEY
C
      CALL PAGES
      X0=12.8
      Y0= 8.5
      R = 5.0
      NTHMAX=100
      PI=ASIN(1.0)*2.0
      DTH=2.0*PI/NTHMAX
      TH=0.0
      X1=X0+R*COS(TH)
      Y1=Y0+R*SIN(TH)
      X2=X0-R*COS(TH)
      Y2=Y0-R*SIN(TH)
C
      CALL GF_SET_GCFUNC(6)
      CALL MOVE(X1,Y1)
      CALL DRAW(X2,Y2)
C
      DO NTH=1,NTHMAX
         TH=DTH*NTH
         XN1=X0+R*COS(TH)
         YN1=Y0+R*SIN(TH)
         XN2=X0-R*COS(TH)
         YN2=Y0-R*SIN(TH)
         CALL MOVE(X1,Y1)
         CALL DRAW(X2,Y2)
         CALL MOVE(XN1,YN1)
         CALL DRAW(XN2,YN2)
         X1=XN1
         Y1=YN1
         X2=XN2
         Y2=YN2
         CALL GF_SYNC
         CALL GU_SLEEP(0.01)
      ENDDO
      CALL MOVE(X1,Y1)
      CALL DRAW(X2,Y2)
      CALL GF_SET_GCFUNC(3)
      CALL PAGEE
C
      DO IC=1,3
      CALL PAGES
         CALL SETLNW(0.2)
         XR(1)=5.0
         YR(1)=2.0
         XR(2)=7.0
         YR(2)=2.0
         XR(3)=7.0
         YR(3)=12.0
         XR(4)=5.0
         YR(4)=12.0
         CALL SETRGB(0.0,0.0,0.0)
         CALL POLY(XR,YR,4)
         XR(1)=XR(1)+5.0
         XR(2)=XR(2)+5.0
         XR(3)=XR(3)+5.0
         XR(4)=XR(4)+5.0
         CALL SETRGB(1.0,0.0,0.0)
         CALL POLY(XR,YR,4)
         CALL POLY(XR,YR,4)
         XR(1)=XR(1)+5.0
         XR(2)=XR(2)+5.0
         XR(3)=XR(3)+5.0
         XR(4)=XR(4)+5.0
         CALL SETRGB(0.0,1.0,0.0)
         CALL POLY(XR,YR,4)
         CALL POLY(XR,YR,4)
         XR(1)=XR(1)+5.0
         XR(2)=XR(2)+5.0
         XR(3)=XR(3)+5.0
         XR(4)=XR(4)+5.0
         CALL SETRGB(0.0,0.0,1.0)
         CALL POLY(XR,YR,4)
C
         IF(IC.EQ.1) THEN
            CALL SETRGB(1.0,1.0,1.0)
         ELSEIF(IC.EQ.2) THEN
            CALL SETRGB(0.0,0.0,0.0)
         ELSE
            CALL SETRGB(1.0,0.0,0.0)
         ENDIF
         X1=3.0
         X2=24.0
         DO I=0,15
            Y=11.0-I*0.5
            CALL GF_SET_GCFUNC(I)
            CALL MOVE(X1,Y)
            CALL DRAW(X2,Y)
         ENDDO
      CALL GF_SET_GCFUNC(3)
      CALL PAGEE
      ENDDO
C
      CALL GSCLOS
      STOP
      END
