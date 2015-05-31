C
C     ******************************
C     ****** BSC TEST PROGRAM ******
C     ******************************
C
      IMPLICIT NONE
      CHARACTER KMENU*1
C
      CALL GSOPEN
C
    1 WRITE(6,*) '# INPUT : B/BUTTONS T/TEXT S/SPIN'
      READ(5,'(A1)',END=9000,ERR=1) KMENU
      CALL GUCPTL(KMENU)
C
      IF(KMENU.EQ.'B') THEN
         CALL PAGES
         CALL BUTTONS
         CALL PAGEE
      ELSEIF(KMENU.EQ.'T') THEN
         CALL PAGES
         CALL XTEXT
         CALL PAGEE
      ELSEIF(KMENU.EQ.'S') THEN
         CALL PAGES
         CALL SPIN
         CALL PAGEE
      ENDIF
      GOTO 1
C
 9000 CALL GSCLOS
      STOP
      END
C
C      
C
      SUBROUTINE BUTTONS
      IMPLICIT NONE
      COMMON /DATA1/ RGB(3)
      REAL RGB, W, H
      EXTERNAL CLICKED, REDRAW, CLEAR, MYSETRGB, CLEAR_REDRAW
      CHARACTER*256 MENU(5)
      MENU(1) = '@black@'
      MENU(2) = '@red@'
      MENU(3) = '@blue@'
      MENU(4) = '@yellow@'
      MENU(5) = '@green@'
C
      RGB(1) = 0.0
      RGB(2) = 0.0
      RGB(3) = 0.0
C	
      CALL REDRAW ()
C	
      CALL SETFNT (2)
      CALL SETCHR (0.35, 0.16, 0.35, 0.0, 0.0)
C
      CALL GUIBUTTON (1.0, 17.0, 2.0, 1.0, '@click@', CLICKED)
      CALL GUIBUTTON (1.0, 16.0, 2.0, 1.0, '@redraw@', REDRAW)
      CALL GUIBUTTON (1.0, 15.0, 2.0, 1.0, '@clear@', CLEAR)
      CALL INQLABELSIZE ('@clear and redraw@', W, H)
      CALL GUIBUTTON (1.0, 14.0, W+0.5, 1.0, '@clear and redraw@',
     &     CLEAR_REDRAW)
      CALL GUILABEL (1.0, 13.0, 2.0, 1.0, '@ color = @', 0)
      CALL GUICOMBOBOX (3.0, 13.0, 2.0, 1.0, 5, MENU, 1, MYSETRGB)

      CALL GUIEXTERNAL (1)

      CALL GUIBUTTON (1.0, 7.0, 2.0, 1.0, '@click@', CLICKED)
      CALL GUIBUTTON (1.0, 6.0, 2.0, 1.0, '@redraw@', REDRAW)
      CALL GUIBUTTON (1.0, 5.0, 2.0, 1.0, '@clear@', CLEAR)
      CALL INQLABELSIZE ('@clear and redraw@', W, H)
      CALL GUIBUTTON (1.0, 4.0, W+0.5, 1.0, '@clear and redraw@',
     &     CLEAR_REDRAW)
      CALL GUILABEL (1.0, 3.0, 2.0, 1.0, '@ color = @', 0)
      CALL GUICOMBOBOX (3.0, 3.0, 2.0, 1.0, 5, MENU, 1, MYSETRGB)
C
      RETURN
      END
C
C
C
      SUBROUTINE XTEXT
      IMPLICIT NONE
      COMMON /DATA2/ IFNT, TEXT, LTEXT, CHS, CHECKED
      REAL CHS, W, H
      INTEGER IFNT, LTEXT
      LOGICAL CHECKED
      CHARACTER*256 TEXT
      EXTERNAL BOUNDINGBOX, DRAWTEXT, MYSETFNT, MYSETCHS
C
      TEXT = '$frac{A}{B}1234'
      IFNT = 2
      CHS = 0.35
      CHECKED = .FALSE.
      CALL SETFNT (IFNT)
      CALL SETCHR (0.35, 0.1, 0.35, 0.0, 0.0)
      CALL DRAWTEXT (TEXT, 15)
C      
      CALL INQLABELSIZE ('@Draw bounding box@', W, H)
      CALL GUICHECKBUTTON (1.0, 13.0, W+1.0, 1.0, '@Draw bounding box@',
     &     CHECKED, BOUNDINGBOX)
      CALL GUILINEEDIT (1.0, 17.0, 6.0, 1.0, 40, TEXT, 15, DRAWTEXT)
      
      CALL INQLABELSIZE ('@Font number = #@', W, H)
      CALL GUILABEL (1.0, 15.0, W, 1.0, '@Font number = #@', 0)
      CALL GUISPINBUTTONI (1.1+W, 15.0, 2.0, 1.0, 2, 0, 19, MYSETFNT)
      CALL INQLABELSIZE ('@Font size = @', W, H)
      CALL GUILABEL (1.0, 14.0, W, 1.0, '@Font size = @', 0)
      CALL GUISPINBUTTONF(1.1+W, 14.0, 2.0, 1.0, 0.35, 0.0, 10.0, 2,
     &     MYSETCHS)
      CALL GUILABEL (3.2+W, 14.0, 1.0, 1.0, '@cm@', 0)
      RETURN
      END
C
C
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
C
C
      SUBROUTINE CLICKED ()
      IMPLICIT NONE
      COMMON /DATA1/ RGB(3)
      REAL RGB
      CALL SETRGB (RGB(1), RGB(2), RGB(3))
      CALL GTEXTX (10.0, 10.0, '@clicked!!@', 0)
      RETURN
      END
C
C
C
      SUBROUTINE REDRAW ()
      IMPLICIT NONE
      COMMON /DATA1/ RGB(3)
      REAL RGB
      CALL SETRGB (RGB(1), RGB(2), RGB(3))
      CALL XMARK ()
      RETURN
      END
C
C
C
      SUBROUTINE CLEAR_REDRAW ()
      IMPLICIT NONE
      COMMON /DATA1/ RGB(3)
      REAL RGB
      CALL ERAS ()
      CALL SETRGB (RGB(1), RGB(2), RGB(3))
      CALL XMARK ()
      RETURN
      END
C
C
C
      SUBROUTINE DRAWTEXT (TEXT1, L)
      IMPLICIT NONE
      COMMON /DATA2/ IFNT, TEXT, LTEXT, CHS, CHECKED
      REAL CHS
      INTEGER IFNT, LTEXT
      LOGICAL CHECKED
      CHARACTER*256 TEXT
      CHARACTER*256 TEXT1
      INTEGER I, L
C
      LTEXT = L
      DO I = 1, L
         TEXT(I:I) = TEXT1(I:I) 
      ENDDO
C
      CALL DRAWTEXT_COMMON
      RETURN
      END
C
C
C
      SUBROUTINE DRAWTEXT_COMMON
      IMPLICIT NONE
      COMMON /DATA2/ IFNT, TEXT, LTEXT, CHS, CHECKED
      REAL CHS, SIZE, HEIGHT, DEPTH
      INTEGER IFNT, LTEXT
      LOGICAL CHECKED
      CHARACTER*256 TEXT
C
      CALL ERAS ()
      IF (LTEXT.GT.0) THEN
         CALL SETFNT (IFNT)
         CALL SETCHR (CHS, CHS * 0.16 / 0.35, CHS, 0.0, 0.0)
         
         IF (CHECKED) THEN
            CALL INQTEXTBBOX (TEXT, LTEXT, SIZE, HEIGHT, DEPTH)
            CALL MOVE (15.0, 10.0+DEPTH)
            CALL DRAW (15.0+SIZE, 10.0+DEPTH)
            CALL DRAW (15.0+SIZE, 10.0+HEIGHT)
            CALL DRAW (15.0, 10.0+HEIGHT)
            CALL DRAW (15.0, 10.0+DEPTH)
         ENDIF
         CALL GTEXT (15.0, 10.0, TEXT, LTEXT, 0)
      ENDIF
      RETURN
      END
C
C
C
      SUBROUTINE BOUNDINGBOX (I)
      IMPLICIT NONE
      COMMON /DATA2/ IFNT, TEXT, LTEXT, CHS, CHECKED
      REAL CHS
      INTEGER IFNT, LTEXT
      LOGICAL CHECKED
      CHARACTER*256 TEXT
      INTEGER I
C
      IF (I.EQ.0) THEN
         CHECKED = .FALSE.
      ELSE
         CHECKED = .TRUE.
      ENDIF
      CALL DRAWTEXT_COMMON
      RETURN
      END
C
C
C
      SUBROUTINE CLEAR ()
      IMPLICIT NONE
      CALL ERAS ()
      RETURN
      END
C
C
C
      SUBROUTINE MYSETRGB (I)
      IMPLICIT NONE
      COMMON /DATA1/ RGB(3)
      REAL RGB
      INTEGER I
C
      IF (I.EQ.1) THEN
         RGB(1) = 0.0
         RGB(2) = 0.0
         RGB(3) = 0.0
      ELSE IF (I.EQ.2) THEN
         RGB(1) = 1.0
         RGB(2) = 0.0
         RGB(3) = 0.0
      ELSE IF (I.EQ.3) THEN
         RGB(1) = 0.0
         RGB(2) = 0.0
         RGB(3) = 1.0
      ELSE IF (I.EQ.4) THEN
         RGB(1) = 1.0
         RGB(2) = 1.0
         RGB(3) = 0.0
      ELSE IF (I.EQ.5) THEN
         RGB(1) = 0.0
         RGB(2) = 1.0
         RGB(3) = 0.0
      ENDIF
      RETURN
      END
C
C
C
      SUBROUTINE MYSETFNT (IFNT1)
      IMPLICIT NONE
      COMMON /DATA2/ IFNT, TEXT, LTEXT, CHS, CHECKED
      REAL CHS
      INTEGER IFNT, LTEXT, IFNT1
      LOGICAL CHECKED
      CHARACTER*256 TEXT
C
      IFNT = IFNT1
C
      CALL DRAWTEXT_COMMON
      RETURN
      END
C
C
C
      SUBROUTINE MYSETCHS (CH)
      IMPLICIT NONE
      COMMON /DATA2/ IFNT, TEXT, LTEXT, CHS, CHECKED
      REAL CHS, CH
      INTEGER IFNT, LTEXT
      LOGICAL CHECKED
      CHARACTER*256 TEXT
C      
      CHS = CH
      CALL DRAWTEXT_COMMON
      RETURN
      END
C
C
C
      SUBROUTINE SPIN
      IMPLICIT NONE
      REAL F
      REAL GXMIN, GXMAX, GYMIN, GYMAX, GZMIN, GZMAX
      REAL X(101),Y(101)
      INTEGER I, J
      COMMON /PERS/ OX, OY, OZ, RADIUS, THETA, PHI,
     &     XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX, 
     &     XSCAL, YSCAL, ZSCAL, Z, IXMAX, IYMAX,
     &     XL, YL, ZL
      REAL OX, OY, OZ, PHI, THETA, RADIUS, Z(101,101)
      REAL XSCAL, YSCAL, ZSCAL, XL, YL, ZL
      REAL XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX, W, H
      INTEGER IXMAX, IYMAX
      EXTERNAL SETPHI
C
      IXMAX=31
      IYMAX=31
C
      F = 1.E+0
C
      DO I=1,IXMAX
      DO J=1,IYMAX
         X(I)=0.2*FLOAT(I-1)-5.0
         Y(J)=0.1*FLOAT(J-1)-5.0
         Z(I,J)=(1.5*COS(X(I))*SIN(Y(J))-0.6)*F
      ENDDO
      ENDDO
      ZMIN= -1.0*F
      ZMAX= 1.0*F
C
      XL=15.0
      YL=15.0
      ZL=6.0
C
      PHI=30.0
      THETA=45.0
      RADIUS=15.0
C
      CALL GMNMX1 (X,1,IXMAX,1,XMIN,XMAX)
      CALL GMNMX1 (Y,1,IYMAX,1,YMIN,YMAX)
      CALL GQSCAL (XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL (YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL (ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
      OX = 0.5*(XMIN+XMAX)
      OY = 0.5*(YMIN+YMAX)
      OZ = 0.5*(ZMIN+ZMAX)
C     ------------------------------------------------------------
      CALL DRAW_PERS ()
C
      CALL GUIEXTERNAL(1)
      CALL INQLABELSIZE ('@PHI = @', W, H)
      CALL GUILABEL (1.0, 16.0, W, 1.0, '@PHI = @', 0)
      CALL GUISPINBUTTONI(1.0+W+0.1, 16.0, 2.0, 1.0, 30, 0, 360, SETPHI)
C
      RETURN
      END
C
C
C
      SUBROUTINE DRAW_PERS ()
      IMPLICIT NONE
      COMMON /PERS/ OX, OY, OZ, RADIUS, THETA, PHI,
     &     XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX, 
     &     XSCAL, YSCAL, ZSCAL, Z, IXMAX, IYMAX,
     &     XL, YL, ZL
      REAL OX, OY, OZ, PHI, THETA, RADIUS, Z(101,101)
      REAL XSCAL, YSCAL, ZSCAL, XL, YL, ZL
      REAL XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX
      INTEGER IXMAX, IYMAX
      EXTERNAL R2G2B
C
      CALL SETPAGE (-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS(0.3, 0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 3, -1)
C     --- Gouraud Shading ---
      CALL PERS3D1 (Z,101,IXMAX,IYMAX,3,R2G2B) 
      CALL GAxis3D (2)
      CALL GDrawBack3D (.3,.3,.3)
C
      RETURN
      END
C
C
C
      SUBROUTINE SETPHI(I)
      IMPLICIT NONE
      COMMON /PERS/ OX, OY, OZ, RADIUS, THETA, PHI,
     &     XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX, 
     &     XSCAL, YSCAL, ZSCAL, Z, IXMAX, IYMAX,
     &     XL, YL, ZL
      REAL OX, OY, OZ, PHI, THETA, RADIUS, Z(101,101)
      REAL XSCAL, YSCAL, ZSCAL, XL, YL, ZL
      REAL XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX
      INTEGER IXMAX, IYMAX
      INTEGER I
C
      PHI = FLOAT(I)
      CALL ERAS ()
      CALL DRAW_PERS ()
      RETURN
      END

