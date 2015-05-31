C
C     SAMPLE PROGRAM
C
      PARAMETER (NXMAX=1001,NYMAX=1001)
      DIMENSION Z(NXMAX,NYMAX),X(NXMAX),Y(NYMAX),
     &          X2(NXMAX,NYMAX),Y2(NXMAX,NYMAX)
      DIMENSION IELM(3,NXMAX)
      DIMENSION Z3(NXMAX),X3(NXMAX),Y3(NXMAX)
      EXTERNAL R2G2B,R2W2B,BLACK,WHITE,R2G2B_17
C
      IXMAX=31
      IYMAX=31
C
      CALL GSOPEN
C
      F = 1.E+20
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
      PHI=45.0
      THETA=45.0
      RADIUS=15.0
 2    WRITE (6,*) 'INPUT PHI,THETA'
      READ (5,*,END=9999,ERR=2) PHI, THETA
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
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS(0.3, 0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
C
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 3, -1)
C     --- Gouraud Shading ---
      CALL PERS3D1 (Z,NXMAX,IXMAX,IYMAX,3,R2G2B) 
      CALL GAxis3D (2)
      CALL GDrawBack3D (.3,.3,.3)
      CALL PAGEE
C     ------------------------------------------------------------
      CALL PAGES
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS(0.35,0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
C
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 1, -1)
C     --- Flat Shading ---
      CALL PERS3D1 (Z, NXMAX, IXMAX, IYMAX, 8, R2G2B_17)
      CALL GAxis3D (2)
      CALL GDrawBack3D (.3,.3,.3)
      CALL PAGEE
C     ------------------------------------------------------------
      IXMAX=33
      IYMAX=33
      DX = 6.0/IXMAX
      DY = 4.0/IYMAX
      DO I=1,IXMAX
      DO J=1,IYMAX
         IF (I.le.2*IXMAX/3-1) THEN
            X(I) = (I-1)*(3.0/IXMAX)
         ELSE
            X(I) = (I-2*IXMAX/3)*(12.0/IXMAX)+2.0
         ENDIF
         IF (J.le.IYMAX/3+1) THEN
            Y(J) = (J-1)*(6.0/IYMAX)
         ELSE
            Y(J) = (J-IYMAX/3)*(3.0/IYMAX)+2.0
         ENDIF
         Z(I,J) = (EXP(-(X(I)-1.0)**2-(Y(J)-3.0)**2))*F
      ENDDO
      ENDDO
      CALL GMNMX1 (X,1,IXMAX,1,XMIN,XMAX)
      CALL GMNMX1 (Y,1,IYMAX,1,YMIN,YMAX)
      CALL GQSCAL (XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL (YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL (ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
      OX = 0.5*(XMIN+XMAX)
      OY = 0.5*(YMIN+YMAX)
      OZ = 0.5*(ZMIN+ZMAX)
C     -----------------------------------------------------------
      CALL PAGES
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS (0.35,0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
C
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 1, -1)
C     --- Gouraud Shading ---
      CALL PERS3D2 (Z, X, Y, NXMAX, IXMAX, IYMAX, 3, R2G2B)
      CALL GAxis3D (0)
      CALL GDrawBack3D (0.3, 0.3, 0.3)
      CALL PAGEE
C     ------------------------------------------------------------
      IXMAX=33
      IYMAX=33
      DX = 6.0/IXMAX
      DY = 4.0/IYMAX
      DO I=1,IXMAX
         DO J=1,IYMAX
            X(I) = 0.2*(I-1)
            Z(I,J) = 0.7 * EXP(-0.5*(X(I)-2.0)**2) + 0.1
         ENDDO
      ENDDO
      CALL GMNMX1(X,1,IXMAX,1,XMIN,XMAX)
      XMIN = -XMAX
      YMIN = XMIN
      YMAX = XMAX
      ZMIN = 0.0
      ZMAX = 1.0
      CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL(ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
      OX = 0.5*(XMIN+XMAX)
      OY = 0.5*(YMIN+YMAX)
      OZ = 0.5*(ZMIN+ZMAX)
C     ------------------------------------------------------------
      CALL PAGES
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS (0.50,0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
C
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 1, 1)
C     --- Gouraud Shading ---
      CALL PERS3D3 (Z, X, NXMAX, IXMAX, IYMAX, 3, R2G2B)
      CALL GAxis3D (0)
      CALL GDrawBack3D (0.3, 0.3, 0.3)
      CALL PAGEE
C     ------------------------------------------------------------
      IXMAX=33
      IYMAX=33
      DT = 120.0/(IYMAX-1)
      DO I=1,IXMAX
         DO J=1,IYMAX
            X(I) = 0.2*(I-1)
            Y(J) = 180.0 * ASIN(2.0/(IYMAX-1)*(J-1)-1.0)/3.1415927
            Z(I,J) = 0.7 * EXP(-0.5*(X(I)-2.0)**2) + 0.1
         ENDDO
      ENDDO
      CALL GMNMX1(X,1,IXMAX,1,XMIN,XMAX)
      XMIN = 0.0
      YMIN = -XMAX
      YMAX = XMAX
      ZMIN = 0.0
      ZMAX = 1.0
      CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL(ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
      OX = 0.5*(XMIN+XMAX)
      OY = 0.5*(YMIN+YMAX)
      OZ = 0.5*(ZMIN+ZMAX)
C     ------------------------------------------------------------
      CALL PAGES
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS (0.50,0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (OX, OY, OZ, RADIUS, THETA, PHI)
C
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 1, 1)
C     --- Gouraud Shading ---
      CALL PERS3D4 (Z, X, Y, NXMAX, IXMAX, IYMAX, 3, R2W2B)
      CALL GAxis3D (0)
      CALL GDrawBack3D (0.3, 0.3, 0.3)
      CALL PAGEE
C     ------------------------------------------------------------
      IXMAX=31
      IYMAX=31
      DY = 2.0 * 3.1415927 / REAL(IYMAX-1)
      DO I=1,IXMAX
         DO J=1,IYMAX
            X2(I,J) = 0.3 * I * (2.0  + cos((J-1)*DY))
            Y2(I,J) = I * (2.0  + sin((J-1)*DY))
            Z (I,J) = 0.7 * SIN(0.1*X2(I,J)*Y2(I,J))
         ENDDO
      ENDDO
      CALL GMNMX2(X2,NXMAX,1,IXMAX,1,1,IYMAX,1,XMIN,XMAX)
      CALL GMNMX2(Y2,NXMAX,1,IXMAX,1,1,IYMAX,1,YMIN,YMAX)
      CALL GMNMX2(Z, NXMAX,1,IXMAX,1,1,IYMAX,1,ZMIN,ZMAX)
      CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL(ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
      OX = 0.5*(XMIN+XMAX)
      OY = 0.5*(YMIN+YMAX)
      OZ = 0.5*(ZMIN+ZMAX)
C     ------------------------------------------------------------
      CALL PAGES
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS (0.35,0.0)
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL GVIEW3D (PHI, THETA, RADIUS, OX, OY, OZ)

      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 1, 1)
C     --- Gouraud Shading ---
      CALL PERS3D5 (Z, X2, Y2, NXMAX, IXMAX, IYMAX, -7, R2W2B)
      CALL GAxis3D (0)
      CALL GDrawBack3D (0.3, 0.3, 0.3)
      CALL PAGEE
C     ------------------------------------------------------------
      NNODE = 11 * 5 + 5 * 4
      NELM = 10 * 4 * 2 + 4 * 4 * 2
      DO I=1, 11
         X3(I) = REAL(I-1)
         X3(I+11) = REAL(I-1)
         X3(I+22) = REAL(I-1)
         X3(I+33) = REAL(I-1)
         X3(I+44) = REAL(I-1)
      ENDDO
      DO I=1, 5
         X3(I+55) = REAL(I-1) + 4
         X3(I+60) = REAL(I-1) + 4
         X3(I+65) = REAL(I-1) + 4
         X3(I+70) = REAL(I-1) + 4
      ENDDO
      DO I=1, 11
         Y3(I) = 0.0
         Y3(I+11) = 1.0
         Y3(I+22) = 2.0
         Y3(I+33) = 3.0
         Y3(I+44) = 4.0
      ENDDO
      DO I=1, 5
         Y3(I+55) = 5.0
         Y3(I+60) = 6.0
         Y3(I+65) = 7.0
         Y3(I+70) = 8.0
      ENDDO
      DO I=1, NNODE
         Z3 (I) = 0.7 * SIN(0.1*X3(I)*Y3(I))
      ENDDO
      DO I=1, 20
         IF (MOD(I,2).EQ.1) THEN
            IELM(1,I) = (I+1)/2
            IELM(2,I) = 12+(I+1)/2
            IELM(3,I) = 11+(I+1)/2
            IELM(1,I+20) = 11+(I+1)/2
            IELM(2,I+20) = 23+(I+1)/2
            IELM(3,I+20) = 22+(I+1)/2
            IELM(1,I+40) = 22+(I+1)/2
            IELM(2,I+40) = 34+(I+1)/2
            IELM(3,I+40) = 33+(I+1)/2
            IELM(1,I+60) = 33+(I+1)/2
            IELM(2,I+60) = 45+(I+1)/2
            IELM(3,I+60) = 44+(I+1)/2
         ELSE
            IELM(1,I) = I/2
            IELM(2,I) = 1+I/2
            IELM(3,I) = 12+I/2
            IELM(1,I+20) = 11+I/2
            IELM(2,I+20) = 12+I/2
            IELM(3,I+20) = 23+I/2
            IELM(1,I+40) = 22+I/2
            IELM(2,I+40) = 23+I/2
            IELM(3,I+40) = 34+I/2
            IELM(1,I+60) = 33+I/2
            IELM(2,I+60) = 34+I/2
            IELM(3,I+60) = 45+I/2
         ENDIF
      ENDDO
      DO I=1, 8
         IF (MOD(I,2).EQ.1) THEN
            IELM(1,I+80) = 48+(I+1)/2
            IELM(2,I+80) = 56+(I+1)/2
            IELM(3,I+80) = 55+(I+1)/2
            IELM(1,I+88) = 60-5+(I+1)/2
            IELM(2,I+88) = 66-5+(I+1)/2
            IELM(3,I+88) = 65-5+(I+1)/2
            IELM(1,I+96) = 65-5+(I+1)/2
            IELM(2,I+96) = 71-5+(I+1)/2
            IELM(3,I+96) = 70-5+(I+1)/2
            IELM(1,I+104) = 70-5+(I+1)/2
            IELM(2,I+104) = 76-5+(I+1)/2
            IELM(3,I+104) = 75-5+(I+1)/2
         ELSE
            IELM(1,I+80) = 48+I/2
            IELM(2,I+80) = 49+I/2
            IELM(3,I+80) = 56+I/2
            IELM(1,I+88) = 60-5+I/2
            IELM(2,I+88) = 61-5+I/2
            IELM(3,I+88) = 66-5+I/2
            IELM(1,I+96) = 65-5+I/2
            IELM(2,I+96) = 66-5+I/2
            IELM(3,I+96) = 71-5+I/2
            IELM(1,I+104) = 70-5+I/2
            IELM(2,I+104) = 71-5+I/2
            IELM(3,I+104) = 76-5+I/2
         ENDIF
      ENDDO
      CALL GMNMX1(X3,1,NNODE,1,XMIN,XMAX)
      CALL GMNMX1(Y3,1,NNODE,1,YMIN,YMAX)
      CALL GMNMX1(Z3,1,NNODE,1,ZMIN,ZMAX)
      CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL(ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
      OX = 0.5*(XMIN+XMAX)
      OY = 0.5*(YMIN+YMAX)
      OZ = 0.5*(ZMIN+ZMAX)
C     ------------------------------------------------------------
      CALL PAGES
      CALL SETPAGE(-0.2*25.6, -0.2*19.0+1.0, 1.4, 1.4)
      CALL SETCHS (0.35,0.0)
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL GVIEW3D (PHI, THETA, RADIUS, OX, OY, OZ)

      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0)
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0)
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0)
      CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
      CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
      CALL GVALUE3DZ (0.0,  ZSCAL, 1, 1)
C     --- Gouraud Shading ---
      CALL PERS3DF (Z3, X3, Y3, NNODE, IELM, NELM, 3, R2G2B)
      CALL GAxis3D (0)
      CALL GDrawBack3D (0.3, 0.3, 0.3)
      CALL PAGEE
C     ------------------------------------------------------------
 9999 CALL GSCLOS
      STOP
      END
