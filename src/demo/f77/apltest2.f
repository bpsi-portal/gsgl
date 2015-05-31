C
      IMPLICIT NONE
      INTEGER MODE
C
      CALL GSOPEN 
C
 1    CONTINUE
      WRITE(6,*) '## INPUT TYPE (1,2,3,4,5,6,7) ?'
      READ(5,*,ERR=1,END=9999) MODE
C
      IF (MODE.EQ.1) THEN
         CALL PAGES
         CALL PATTERN
         CALL PAGEE
      ELSE IF (MODE.EQ.2) THEN
         CALL PAGES
         CALL PATTERN2D
         CALL PAGEE
      ELSE IF (MODE.EQ.3) THEN
         CALL PAGES
         CALL TEXT_IN_2D
         CALL PAGEE
      ELSE IF (MODE.EQ.4) THEN
         CALL PAGES
         CALL PATTERN3D
         CALL PAGEE
      ELSE IF (MODE.EQ.5) THEN
         CALL PAGES
         CALL TEXT_IN_3D
         CALL PAGEE
      ELSE IF (MODE.EQ.6) THEN
         CALL PAGES
         CALL TEXT_IN_3D_2
         CALL PAGEE
      ELSE IF (MODE.EQ.7) THEN
         CALL PAGES
         CALL TEXT_IN_3D_3
         CALL PAGEE
      ENDIF
      GOTO 1
C
 9999 CALL GSCLOS
      STOP
      END
C
C
C
      SUBROUTINE PATTERN
      IMPLICIT NONE
      INTEGER IPAT
      DO IPAT = 0, 7
         CALL MOVEPT (1.0, 16.0 - IPAT * 2, IPAT)
         CALL DRAWPT (24.0, 16.0 - IPAT * 2)
      ENDDO
      RETURN
      END
C
C
C
      SUBROUTINE PATTERN2D
      IMPLICIT NONE
      INTEGER IPAT
      CALL GDEFIN (0.0, 25.6, 0.0, 18.1,
     &             0.0, 1.0, 0.0, 1.0)
      DO IPAT = 0, 7
         CALL MOVEPT2D (0.1, 0.9 - IPAT * 0.12, IPAT)
         CALL DRAWPT2D (0.9, 0.9 - IPAT * 0.12)
      ENDDO
      RETURN
      END
C
C
C
      SUBROUTINE TEXT_IN_2D
      IMPLICIT NONE
C
      CALL GDEFIN (0.0, 25.6, 0.0, 18.1,
     &             0.0, 1.0, 0.0, 1.0)
      CALL MOVE2D (0.1, 0.1)
      CALL DRAW2D (0.2, 0.2)
      CALL TEXT2D ('Text2D', 6)
C
      CALL GTEXT2D (0.2, 0.4, 'GTEXT2D 1.0', 11, 0)
      CALL GTEXT2D (0.2, 0.43, 'GTEXT2D 1.0', 11, 1)
      CALL GTEXT2D (0.2, 0.46, 'GTEXT2D 1.0', 11, 2)
      CALL GTEXT2D (0.2, 0.49, 'GTEXT2D 1.0', 11, 3)
C
      CALL GTEXTX2D (0.2, 0.6, '@$frac{dy}{dx} = f(x)@', 0)
C
      CALL GNUMBI2D (0.6, 0.10, 12345, 0)
      CALL GNUMBI2D (0.6, 0.13, 12345, 1)
      CALL GNUMBI2D (0.6, 0.16, 12345, 2)
      CALL GNUMBR2D (0.6, 0.20, 12345.678, 3, 0)
      CALL GNUMBR2D (0.6, 0.23, 12345.678, 3, 1)
      CALL GNUMBR2D (0.6, 0.26, 12345.678, 3, 2)
      CALL GNUMBR2D (0.6, 0.29, 12345.678, 3, 3)
      CALL GNUMBR2D (0.6, 0.40, 123.456, -3, 0)
      CALL GNUMBR2D (0.6, 0.43, 123.456, -3, 1)
      CALL GNUMBR2D (0.6, 0.46, 123.456, -3, 2)
      CALL GNUMBR2D (0.6, 0.49, 123.456, -3, 3)
      CALL GNUMBR2D (0.6, 0.60, 123.456, -103, 0)
      CALL GNUMBR2D (0.6, 0.63, 123.456, -103, 1)
      CALL GNUMBR2D (0.6, 0.66, 123.456, -103, 2)
      CALL GNUMBR2D (0.6, 0.69, 123.456, -103, 3)
      CALL GNUMBP2D (0.6, 0.80, 10, 16, 0)
      CALL GNUMBP2D (0.6, 0.83, 10, 16, 1)
      CALL GNUMBP2D (0.6, 0.86, 10, 16, 2)
      CALL GNUMBP2D (0.6, 0.89, 10, 16, 3)
C
      RETURN
      END
C
C
C
      SUBROUTINE PATTERN3D
      IMPLICIT NONE
      INTEGER IPAT
      REAL XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX
      REAL OX, OY, OZ, RADIUS, THETA, PHI
C
      XL = 15.0
      YL = 15.0
      ZL = 15.0
      XMIN = 0.0
      XMAX = 1.0
      YMIN = 2.0
      YMAX = 4.0
      ZMIN = 100.0
      ZMAX = 200.0
C
      OX = 0.5 * (XMIN + XMAX)
      OY = 0.5 * (YMIN + YMAX)
      OZ = 0.5 * (ZMIN + ZMAX)
      RADIUS = 15.0
      THETA = 30.0
      PHI = 30.0
C      
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
      DO IPAT = 0, 7
         CALL MOVEPT3D (0.1, 3.9 - IPAT * 0.24, 150.0, IPAT)
         CALL DRAWPT3D (0.9, 3.9 - IPAT * 0.24, 150.0)
      ENDDO
      RETURN
      END
C
C
C
      SUBROUTINE TEXT_IN_3D
      IMPLICIT NONE
      REAL XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX
      REAL OX, OY, OZ, RADIUS, THETA, PHI
C
      XL = 15.0
      YL = 15.0
      ZL = 15.0
      XMIN = 0.0
      XMAX = 1.0
      YMIN = 0.0
      YMAX = 1.0
      ZMIN = 0.0
      ZMAX = 1.0
C
      OX = 0.5 * (XMIN + XMAX)
      OY = 0.5 * (YMIN + YMAX)
      OZ = 0.5 * (ZMIN + ZMAX)
      RADIUS = 15.0
      THETA = 10.0
      PHI = -60.0
C      
C      CALL SETCHS (1.0, 0.0)
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
      CALL SETPROJECTIONTYPE (0) ! ORTHOGONAL
      CALL SET3DTEXTBASELINE (1.0, 0.0, 0.0, 0.0, 1.0, 0.0)
C
      CALL MOVE3D (0.1, 0.1, 0.1)
      CALL DRAW3D (0.2, 0.2, 0.2)
      CALL TEXT3D ('Text3D', 6)
C
      CALL GTEXT3D (0.2, 0.40, 0.1, 'GTEXT3D 1.0', 11, 0)
      CALL GTEXT3D (0.2, 0.43, 0.1, 'GTEXT3D 1.0', 11, 1)
      CALL GTEXT3D (0.2, 0.46, 0.1, 'GTEXT3D 1.0', 11, 2)
      CALL GTEXT3D (0.2, 0.49, 0.1, 'GTEXT3D 1.0', 11, 3)
C
      CALL GTEXTX3D (0.2, 0.6, 0.1, '@$frac{dy}{dx} = f(x)@', 0)
C
      CALL GNUMBI3D (0.6, 0.10, 0.1, 12345, 0)
      CALL GNUMBI3D (0.6, 0.13, 0.1, 12345, 1)
      CALL GNUMBI3D (0.6, 0.16, 0.1, 12345, 2)
      CALL GNUMBR3D (0.6, 0.20, 0.1, 12345.678, 3, 0)
      CALL GNUMBR3D (0.6, 0.23, 0.1, 12345.678, 3, 1)
      CALL GNUMBR3D (0.6, 0.26, 0.1, 12345.678, 3, 2)
      CALL GNUMBR3D (0.6, 0.29, 0.1, 12345.678, 3, 3)
      CALL GNUMBR3D (0.6, 0.40, 0.1, 123.456, -3, 0)
      CALL GNUMBR3D (0.6, 0.43, 0.1, 123.456, -3, 1)
      CALL GNUMBR3D (0.6, 0.46, 0.1, 123.456, -3, 2)
      CALL GNUMBR3D (0.6, 0.49, 0.1, 123.456, -3, 3)
      CALL GNUMBR3D (0.6, 0.60, 0.1, 123.456, -103, 0)
      CALL GNUMBR3D (0.6, 0.63, 0.1, 123.456, -103, 1)
      CALL GNUMBR3D (0.6, 0.66, 0.1, 123.456, -103, 2)
      CALL GNUMBR3D (0.6, 0.69, 0.1, 123.456, -103, 3)
      CALL GNUMBP3D (0.6, 0.80, 0.1, 10, 16, 0)
      CALL GNUMBP3D (0.6, 0.83, 0.1, 10, 16, 1)
      CALL GNUMBP3D (0.6, 0.86, 0.1, 10, 16, 2)
      CALL GNUMBP3D (0.6, 0.89, 0.1, 10, 16, 3)
      RETURN
      END
C
C
C
      SUBROUTINE TEXT_IN_3D_2
      ZMIN = -1.0
      ZMAX =  1.0
      XMIN = -1.0
      XMAX =  1.0
      YMIN = -1.0
      YMAX =  1.0
C
      XL=10.0
      YL=10.0
      ZL=10.0
C
      PHI=50.0
      THETA=60.0
      RADIUS=10.0
C
      CALL SETCHS(0.50,0.0)
C
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (
     &     0.5*(XMIN+XMAX),0.5*(YMIN+YMAX),0.5*(ZMIN+ZMAX),
     &     RADIUS, THETA, PHI)
C
      CALL SET3DTEXTBASELINE (1.0, 0.0, 0.0, 0.0, 1.0, 0.0)
      CALL GTEXT3D (0.0, 0.0, 1.0, 'X-Y PLANE (Z=1.0)', 17, 2)
      CALL SET3DTEXTBASELINE (-1.0, 0.0, 0.0, 0.0, 1.0, 0.0)
      CALL GTEXT3D (0.0, 0.0,-1.0, 'X-Y PLANE (Z=-1.0)', 18, 2)
C
      CALL SET3DTEXTBASELINE (0.0, 1.0, 0.0, 0.0, 0.0, 1.0)
      CALL GTEXT3D (1.0, 0.0, 0.0, 'Y-Z PLANE (X=1.0)', 17, 2)
      CALL SET3DTEXTBASELINE (0.0, -1.0, 0.0, 0.0, 0.0, 1.0)
      CALL GTEXT3D (-1.0, 0.0, 0.0, 'Y-Z PLANE (X=-1.0)', 18, 2)
C
      CALL SET3DTEXTBASELINE (0.0, 1.0, 0.0, 0.0, 0.0, 1.0)
      CALL GTEXT3D (1.0, 0.0, 0.0, 'Y-Z PLANE (X=1.0)', 17, 2)
      CALL SET3DTEXTBASELINE (0.0, -1.0, 0.0, 0.0, 0.0, 1.0)
      CALL GTEXT3D (-1.0, 0.0, 0.0, 'Y-Z PLANE (X=-1.0)', 18, 2)
C
      CALL SET3DTEXTBASELINE (0.0, 0.0, 1.0, 1.0, 0.0, 0.0)
      CALL GTEXT3D (0.0, 1.0, 0.0, 'Z-X PLANE (Y=1.0)', 17, 2)
      CALL SET3DTEXTBASELINE (0.0, 0.0,-1.0, 1.0, 0.0, 0.0)
      CALL GTEXT3D (0.0,-1.0, 0.0, 'Z-X PLANE (Y=-1.0)', 18, 2)
C
      CALL SET3DTEXTBASELINE (0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
      CALL SETRGB (0.0, 0.0, 1.0)
      CALL MOVE3D (-1.5, 0.0, 0.0)
      CALL DRAW3D (1.5, 0.0, 0.0)
      CALL GTEXT3D (1.6, 0.0, 0.0, 'X-AXIS', 6, 2)
      CALL MOVE3D (0.0,-1.5, 0.0)
      CALL DRAW3D (0.0, 1.5, 0.0)
      CALL GTEXT3D (0.0, 1.6, 0.0, 'Y-AXIS', 6, 2)
      CALL MOVE3D (0.0, 0.0,-1.5)
      CALL DRAW3D (0.0, 0.0, 1.5)
      CALL GTEXT3D (0.0, 0.0, 1.6, 'Z-AXIS', 6, 2)
C
      CALL GAXIS3D(2)
C
      RETURN
      END
C
C
C
      SUBROUTINE TEXT_IN_3D_3
C
      ZMIN = -1.0
      ZMAX =  1.0
      XMIN = -1.0
      XMAX =  1.0
      YMIN = -1.0
      YMAX =  1.0
C
      XL=10.0
      YL=10.0
      ZL=10.0
C
      PHI=50.0
      THETA=60.0
      RADIUS=10.0
C
      CALL SETCHS(1.0,0.0)
      CALL GDEFIN3D(XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (
     &     0.5*(XMIN+XMAX),0.5*(YMIN+YMAX),0.5*(ZMIN+ZMAX),
     &     RADIUS, THETA, PHI)
C     
      PI = ATAN(1.0)*4.0
      DO I = 0, 360, 60
         CALL SETRGB (REAL(I)/360.0, 0.0, 1.0-REAL(I)/360.0)
         CALL SET3DTEXTBASELINE (COS(2.0*PI*REAL(I)/360.0), 
     &        SIN(2.0*PI*REAL(I)/360.0), 0.0, 0.0, 0.0, 1.0)
         CALL MOVE3D (0.0, 0.0, 0.0)
         CALL TEXT3D ('  TEXT IN 3D SPACE', 18)
      ENDDO
C
      RETURN
      END
