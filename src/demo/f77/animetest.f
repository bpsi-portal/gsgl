      include '../../gsglf.h'
      PARAMETER (NMX=101)
      DIMENSION X(NMX), Y(NMX), Z(NMX, NMX)
      DIMENSION VX(3),VY(3),VZ(3)
      EXTERNAL R2G2B
C
      CALL GSOPEN
C     ---------------------------------------------------------
      CALL PAGES
      CALL SETCHS (0.35,0.0)
C
      XMIN = 2.0
      XMAX = 24.0
      X(1) = XMIN
      X(2) = XMIN
      X(3) = XMAX
      X(4) = XMAX
C
      YMIN = 2.0
      YMAX = 17.0
      Y(1) = YMIN
      Y(2) = YMAX
      Y(3) = YMAX
      Y(4) = YMIN      
C
      CALL GDEFIN (XMIN, XMAX, YMIN, YMAX, 0.0, 1.0, 0.0, 1.0)
C      
      DO I = 1, 91
         CALL ANIMES
C
         X(1) = XMIN
         X(2) = XMIN
         X(3) = XMAX
         X(4) = XMAX
         Y(1) = YMIN
         Y(2) = YMAX
         Y(3) = YMAX
         Y(4) = YMIN
         CALL SETRGB (1.0, 1.0, 0.5)
         CALL POLY (X, Y, 4)
C
         X(1) = (I-1) * 0.01 
         X(2) = (I-1) * 0.01 
         X(3) = (I-1) * 0.01 + 0.1
         X(4) = (I-1) * 0.01 + 0.1
         Y(1) = (I-1) * 0.01
         Y(2) = (I-1) * 0.01 + 0.1
         Y(3) = (I-1) * 0.01 + 0.1
         Y(4) = (I-1) * 0.01
         CALL SETRGB (1.0, 0.0, 0.0)
         CALL POLY2D (X, Y, 4)
C
         CALL SETRGB(-1.0, 0.0, 0.0)
         CALL GFRAME
C
         CALL ANIMEE
      ENDDO
C      
      CALL PAGEE
C     -----------------------------------------------
      IXMAX = 31
      IYMAX = 31
C
      DO I=1,IXMAX
      DO J=1,IYMAX
         X(I)=0.3*FLOAT(I-1)-5.0
         Y(J)=0.15*FLOAT(J-1)-5.0
         Z(I,J)=(1.5*COS(X(I))*SIN(Y(J))-0.6)
      ENDDO
      ENDDO
C
      ZMIN= -1.0
      ZMAX= 1.0
C
      XL=15.0
      YL=15.0
      ZL=6.0
C
      PHI=45.0
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
      CALL PAGES
      CALL SETCHS(0.3, 0.0)
C
      CALL gsglProjImgScale (1.4, 1.4)
      CALL gsglProjImgTranslate (0.0, 1.0)
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
C
      DO I = 0, 50
         CALL ANIMES
C
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
         CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 3, -1)
C     --- Gouraud Shading ---
         DO K=1,IXMAX
            DO J=1,IYMAX
               Z(K,J)=(1.5*COS(X(K)+0.2*I)*SIN(Y(J))-0.6)
            ENDDO
         ENDDO
C
         CALL PERS3D1 (Z, NMX, IXMAX, IYMAX, 3, R2G2B) 
         CALL GAxis3D (2)
         CALL ANIMEE
      ENDDO
C
      CALL PAGEE
C     ------------------------------------------------------------
      CALL PAGES
C
      R = 2.0
      XL = GSGL_PAPER_WIDTH
      YL = GSGL_PAPER_HEIGHT
      ZL = GSGL_VIEW_DEPTH
      VX(1) = 1.0
      VX(2) = 0.0
      VX(3) = 0.0
      VY(1) = 0.0
      VY(2) = 1.0
      VY(3) = 0.0
      VZ(1) = 0.0
      VZ(2) = 0.0
      VZ(3) = 1.0
      CALL GDEFIN3D (XL, YL, ZL, 0.0, XL, 0.0, YL, 0.0, ZL)
      OX = 0.5 * XL
      OY = 0.5 * YL
      OZ = 0.5 * ZL
      CALL SET3DCOORD_CAUT1 (OX, OY, OZ, OZ, VX, VY, VZ)
      CALL SETRGB (1.0, 0.0, 0.0)
C
      PHI = GURAND (8.0 * ATAN (1.0))
      THETA = GURAND (4.0 * ATAN (1.0))
      V1 = 1.0 * SIN(THETA) * COS (PHI)
      V2 = 1.0 * SIN(THETA) * SIN (PHI)
      V3 = ABS(1.0 * COS(THETA))
C
      DT = 1.0
      CALL gsglEnableLighting
      DO I = 0, 100
         CALL ANIMES
         CALL SETRGB (1.0, 1.0, 0.5)
         CALL DRAW_BACK (XL, YL, ZL)
         OX = OX + V1 * DT
         OY = OY + V2 * DT
         OZ = OZ + V3 * DT
         IF (OX.LT.R) THEN
            OX = R + (R - OX)
            V1 = -V1
         ELSEIF (OX.GT.XL-R) THEN
            OX = XL - R - (OX - XL + R)
            V1 = -V1
         ENDIF
         IF (OY.LT.R) THEN
            OY = R + (R - OY)
            V2 = -V2
         ELSEIF (OY.GT.YL-R) THEN
            OY = YL - R - (OY - YL + R)
            V2 = -V2
         ENDIF
         IF (OZ.LT.R) THEN
            OZ = R + (R - OZ)
            V3 = -V3
         ELSEIF (OZ.GT.ZL-R) THEN
            OZ = ZL - R - (OZ - ZL + R)
            V3 = -V3
         ENDIF
         CALL SETRGB (1.0, 0.0, 0.0)
         CALL GSGLSOLIDSPHERE (OX, OY, OZ, vx, vy, vz, R, 20, 20)
         CALL ANIMEE
      ENDDO
C
      CALL PAGEE
C     ------------------------------------------------------------
      CALL GSCLOS
      STOP
      END
C
C
C
      SUBROUTINE DRAW_BACK (XL, YL, ZL)
      DIMENSION X(4),Y(4),Z(4)
      EPS = 0.01
      X(1) = 0.0 + EPS
      X(2) = 0.0 + EPS
      X(3) = 0.0 + EPS
      X(4) = 0.0 + EPS
      Y(1) = 0.0 + EPS
      Y(2) = YL - EPS
      Y(3) = YL - EPS
      Y(4) = 0.0 + EPS
      Z(1) = 0.0 + EPS
      Z(2) = 0.0 + EPS
      Z(3) = ZL - EPS
      Z(4) = ZL - EPS
      CALL POLY3D (X, Y, Z, 4)
C
      X(1) = XL - EPS
      X(2) = XL - EPS
      X(3) = XL - EPS
      X(4) = XL - EPS
      Y(1) = 0.0 + EPS
      Y(2) = YL - EPS
      Y(3) = YL - EPS
      Y(4) = 0.0 + EPS
      Z(1) = 0.0 + EPS
      Z(2) = 0.0 + EPS
      Z(3) = ZL - EPS
      Z(4) = ZL - EPS     
      CALL POLY3D (X, Y, Z, 4)
C
      X(1) = 0.0 + EPS
      X(2) = XL - EPS
      X(3) = XL - EPS
      X(4) = 0.0 + EPS
      Y(1) = YL - EPS
      Y(2) = YL - EPS
      Y(3) = YL - EPS
      Y(4) = YL - EPS
      Z(1) = 0.0 + EPS
      Z(2) = 0.0 + EPS
      Z(3) = ZL - EPS
      Z(4) = ZL - EPS     
      CALL POLY3D (X, Y, Z, 4)
C
      X(1) = 0.0 + EPS
      X(2) = XL - EPS
      X(3) = XL - EPS
      X(4) = 0.0 + EPS
      Y(1) = 0.0 + EPS
      Y(2) = 0.0 + EPS
      Y(3) = 0.0 + EPS
      Y(4) = 0.0 + EPS
      Z(1) = 0.0 + EPS
      Z(2) = 0.0 + EPS
      Z(3) = ZL - EPS
      Z(4) = ZL - EPS     
      CALL POLY3D (X, Y, Z, 4)
C
      X(1) = 0.0 + EPS
      X(2) = XL - EPS
      X(3) = XL - EPS
      X(4) = 0.0 + EPS
      Y(1) = 0.0 + EPS
      Y(2) = 0.0 + EPS
      Y(3) = YL - EPS
      Y(4) = YL - EPS
      Z(1) = 2.0 + EPS
      Z(2) = 2.0 + EPS
      Z(3) = 2.0 + EPS
      Z(4) = 2.0 + EPS     
      CALL POLY3D (X, Y, Z, 4)
C
      RETURN
      END
