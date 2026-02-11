C     $Id: apltest.f,v 1.3 2004/08/19 04:39:51 akutsu Exp $
C
      DIMENSION X(51),Y(51),Y1(51),Y2(51)
      INTEGER IOS
C
      CALL GSOPEN
C
      MODE=0
      DX=2*3.1415927/50
      Y0=0.0
      YA=1.0
      GXORG=0.0
      GYORG=0.0
      NXS=9
      NXV=2
      NYS=9
      NYV=2
      NXSL=7
      NXVL=1
      NYSL=9
      NYVL=9
      NM=0
      NP=0
      SCL=0.3
C
    1 WRITE(6,*) '## INPUT TYPE (0..7,8,9,10) ?'
      READ(5,*,IOSTAT=IOS) MODE
      IF (IOS .LT. 0) GOTO 9999
      IF (IOS .GT. 0) GOTO 1
C
      IF(MODE.LE.3) THEN
C
 1000    WRITE(6,*) '# INPUT : Y0,YA,GXORG,GYORG'
         READ(5,*,IOSTAT=IOS) Y0,YA,GXORG,GYORG
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
         WRITE(6,*) '# INPUT : NXS,NYS,NXV,NYV,NM,NP,SCL'
         READ(5,*,IOSTAT=IOS) NXS,NYS,NXV,NYV,NM,NP,SCl
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
C
         DO 1100 N=1,51
           X(N)=FLOAT(N-1)*DX
           Y(N)=Y0+YA*SIN(X(N))
 1100    CONTINUE
         CALL GMNMX1(X,1,51,1,XMIN,XMAX)
         CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,GXSCAL)
         CALL GMNMX1(Y,1,51,1,YMIN,YMAX)
         CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,GYSCAL)
C
         CALL PAGES
         CALL SETCHS(0.35,0.0)
         IF(MODE.EQ.0) THEN
            CALL GDEFIN(4.,24.,2.,17.,GXMIN,GXMAX,GYMIN,GYMAX)
         ELSEIF(MODE.EQ.1) THEN
            CALL GDEFIN(4.,24.,2.,17.,GXMAX,GXMIN,GYMIN,GYMAX)
         ELSEIF(MODE.EQ.2) THEN
            CALL GDEFIN(4.,24.,2.,17.,GXMIN,GXMAX,GYMAX,GYMIN)
         ELSE
            CALL GDEFIN(4.,24.,2.,17.,GXMAX,GXMIN,GYMAX,GYMIN)
         ENDIF
         CALL GFRAME
         CALL GSCALE(GXORG,GXSCAL,0.0,0.0,SCL,NXS)
         CALL GSCALE(0.0,0.0,GYORG,GYSCAL,SCL,NYS)
         CALL GVALUE(GXORG,2*GXSCAL,0.0,0.0,NXV)
         CALL GVALUE(0.0,0.0,GYORG,2*GYSCAL,NYV)
         CALL GPLOTP(X,Y,1,51,1,NM,10,NP)
         CALL PAGEE
         GOTO 1000
C
      ELSEIF(MODE.LE.7) THEN
C
 2000    WRITE(6,*) '# INPUT : Y0,YA,GXORG,GYORG'
         READ(5,*,IOSTAT=IOS) Y0,YA,GXORG,GYORG
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
 2010    WRITE(6,*) '# INPUT : NXS,NYS,NXV,NYV,NM,NP,SCL'
         READ(5,*,IOSTAT=IOS) NXS,NYS,NXV,NYV,NM,NP,SCL
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
         WRITE(6,*) '# INPUT : NXSL,NYSL,NXVL,NYVL'
         READ(5,*,IOSTAT=IOS) NXSL,NYSL,NXVL,NYVL
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
C
         DO 2100 N=1,51
           X(N)=FLOAT(N-1)*DX
           Y(N)=Y0+YA*SIN(X(N))
 2100    CONTINUE
         CALL GMNMX1(X,1,51,1,XMIN,XMAX)
         CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,GXSCAL)
         CALL GMNMX1(Y,1,51,1,YMIN,YMAX)
         CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,GYSCAL)
C
         CALL PAGES
         CALL SETCHS(0.35,0.0)
         IF(MODE.EQ.4) THEN
            CALL GDEFIN(4.,24.,2.,17.,GXMIN,GXMAX,GYMIN,GYMAX)
         ELSEIF(MODE.EQ.5) THEN
            CALL GDEFIN(4.,24.,2.,17.,GXMAX,GXMIN,GYMIN,GYMAX)
         ELSEIF(MODE.EQ.6) THEN
            CALL GDEFIN(4.,24.,2.,17.,GXMIN,GXMAX,GYMAX,GYMIN)
         ELSE
            CALL GDEFIN(4.,24.,2.,17.,GXMAX,GXMIN,GYMAX,GYMIN)
         ENDIF
         CALL GFRAME
         CALL GSCALL(GXORG,NXSL,0.0,0,SCL,NXS)
         CALL GSCALL(0.0,0,GYORG,NYSL,SCL,NYS)
         CALL GVALUL(GXORG,NXVL,0.0,0,NXV)
         CALL GVALUL(0.0,0,GYORG,NYVL,NYV)
         CALL GPLOTP(X,Y,1,51,1,NM,10,NP)
         CALL PAGEE
         GOTO 2000
C
      ELSEIF(MODE.EQ.8) THEN
         CALL PAGES
         CALL SETFNT(2)
         CALL GNUMBR(5.0,15.0, 30.0 ,0,1)
         CALL GNUMBR(5.0,14.0, 30.0, 1,1)
         CALL GNUMBR(5.0,13.0, 30.0,-1,1)
         CALL GNUMBR(5.0,12.0,  3.0 ,0,1)
         CALL GNUMBR(5.0,11.0,  3.0, 1,1)
         CALL GNUMBR(5.0,10.0,  3.0,-1,1)
         CALL GNUMBR(5.0, 9.0,  0.0 ,0,1)
         CALL GNUMBR(5.0, 8.0,  0.0, 1,1)
         CALL GNUMBR(5.0, 7.0,  0.0,-1,1)
         CALL GNUMBR(5.0, 6.0, -3.0 ,0,1)
         CALL GNUMBR(5.0, 5.0, -3.0, 1,1)
         CALL GNUMBR(5.0, 4.0, -3.0,-1,1)
         CALL GNUMBR(5.0, 3.0,-30.0 ,0,1)
         CALL GNUMBR(5.0, 2.0,-30.0, 1,1)
         CALL GNUMBR(5.0, 1.0,-30.0,-1,1)
         CALL GNUMBR(8.0,15.0, 30.0 ,0,0)
         CALL GNUMBR(8.0,14.0, 30.0, 1,0)
         CALL GNUMBR(8.0,13.0, 30.0,-1,0)
         CALL GNUMBR(8.0,12.0,  3.0 ,0,0)
         CALL GNUMBR(8.0,11.0,  3.0, 1,0)
         CALL GNUMBR(8.0,10.0,  3.0,-1,0)
         CALL GNUMBR(8.0, 9.0,  0.0 ,0,0)
         CALL GNUMBR(8.0, 8.0,  0.0, 1,0)
         CALL GNUMBR(8.0, 7.0,  0.0,-1,0)
         CALL GNUMBR(8.0, 6.0, -3.0 ,0,0)
         CALL GNUMBR(8.0, 5.0, -3.0, 1,0)
         CALL GNUMBR(8.0, 4.0, -3.0,-1,0)
         CALL GNUMBR(8.0, 3.0,-30.0 ,0,0)
         CALL GNUMBR(8.0, 2.0,-30.0, 1,0)
         CALL GNUMBR(8.0, 1.0,-30.0,-1,0)
         CALL GTEXT(15.0,15.0,'123',3,0)
         CALL GTEXT(15.0,14.0,'123',3,1)
         CALL GTEXT(15.0,13.0,'12',2,0)
         CALL GTEXT(15.0,12.0,'12',2,1)
         CALL GTEXT(15.0,11.0,'1',1,0)
         CALL GTEXT(15.0,10.0,'1',1,1)
         CALL INQTSZ('123',3,S)
         CALL GNUMBR(15.0, 8.0,S,-3,0)
         CALL INQTSZ('12',2,S)
         CALL GNUMBR(15.0, 7.0,S,-3,0)
         CALL INQTSZ('1',1,S)
         CALL GNUMBR(15.0, 6.0,S,-3,0)         
         CALL PAGEE      
      ELSEIF(MODE.EQ.9) THEN
         CALL PAGES
         CALL SETCHS(0.3,0.0)
         CALL GAXIS2(2.0,2.0,14.0, 2.0,0.0,10.0,0.0,1.0,0.2,0)
         CALL GAXIS2(2.0,2.0,12.0, 8.0,0.0,10.0,0.0,1.0,0.2,1)
         CALL GAXIS2(2.0,2.0, 8.0,12.0,0.0,10.0,0.0,1.0,0.2,2)
         CALL GAXIS2(2.0,2.0, 2.0,14.0,0.0,10.0,0.0,1.0,0.2,3)
         CALL GAXIV2(2.0,2.0,14.0, 2.0,0.0,10.0,0.0,2.0,1, 0)
         CALL GAXIV2(2.0,2.0,12.0, 8.0,0.0,10.0,0.0,2.0,1, 4)
         CALL GAXIV2(2.0,2.0, 8.0,12.0,0.0,10.0,0.0,2.0,1, 8)
         CALL GAXIV2(2.0,2.0, 2.0,14.0,0.0,10.0,0.0,2.0,1,12)
         CALL PAGEE
C
      ELSEIF(MODE.EQ.10) THEN
C
 3000    WRITE(6,*) '# INPUT : Y0,YA,GXORG,GYORG'
         READ(5,*,IOSTAT=IOS) Y0,YA,GXORG,GYORG
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
         WRITE(6,*) '# INPUT : NXS,NYS,NXV,NYV,NM,NP,SCL'
         READ(5,*,IOSTAT=IOS) NXS,NYS,NXV,NYV,NM,NP,SCl
         IF (IOS .NE. 0) THEN
            CALL GS_RESET_STDIN
            GOTO 1
         ENDIF
C
         DO N=1,51
            X(N)=FLOAT(N-1)*DX
            Y(N)=Y0+YA*SIN(X(N))
            Y1(N)=Y(N)+0.1*YA*COS(X(N))
            Y2(N)=Y(N)-0.1*YA*COS(X(N))
         ENDDO
         CALL GMNMX1(X,1,51,1,XMIN,XMAX)
         CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,GXSCAL)
         CALL GMNMX1(Y,1,51,1,YMIN,YMAX)
         CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,GYSCAL)
C
         CALL PAGES
         CALL SETCHS(0.35,0.0)
         CALL GDEFIN(4.,24.,2.,17.,GXMIN,GXMAX,GYMIN,GYMAX)
         CALL GFRAME
         CALL GSCALE(GXORG,GXSCAL,0.0,0.0,SCL,NXS)
         CALL GSCALE(0.0,0.0,GYORG,GYSCAL,SCL,NYS)
         CALL GVALUE(GXORG,2*GXSCAL,0.0,0.0,NXV)
         CALL GVALUE(0.0,0.0,GYORG,2*GYSCAL,NYV)
         CALL GPLOTPE(X,Y1,Y2,1,51,1,0.5)
         CALL GPLOTP(X,Y,1,51,1,NM,10,NP)
         CALL PAGEE
      ENDIF
      GOTO 1
 9999 CALL GSCLOS
      STOP
      END
      SUBROUTINE GS_RESET_STDIN
      INTEGER IOS
      CLOSE(5)
      OPEN(UNIT=5,FILE='/dev/tty',STATUS='OLD',IOSTAT=IOS)
      RETURN
      END
