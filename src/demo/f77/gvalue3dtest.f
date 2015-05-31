C
C     SAMPLE PROGRAM gvalue3dtest.f
C
      ZMIN = -1.0
      ZMAX =  1.0
      XMIN = -2.0
      XMAX =  2.0
      YMIN = -1.0
      YMAX =  1.0
C
      XL=15.0
      YL=15.0
      ZL=6.0
C
      THETA=60.0
      RADIUS=50.0
C
      CALL GQSCAL (XMIN,XMAX,GXMIN,GXMAX,XSCAL)
      CALL GQSCAL (YMIN,YMAX,GYMIN,GYMAX,YSCAL)
      CALL GQSCAL (ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL)
C
      CALL GSOPEN
C     
 2    WRITE (6,*) 'INPUT THETA'
      READ (5,*,END=9999,ERR=2) THETA
C     -------------------------------------------------------------
      DO I = 30, 360, 45
         CALL PAGES
         CALL SETCHS (0.35, 0.0)
         CALL SETFNT (2)
C     
         CALL GTEXTX (22.0, 17.0, "@$#f$#=@", 0)
         CALL INQTSZ ("$#f$#=", 6, SIZE)
         CALL GNUMBI (22.0+SIZE, 17.0, I, 0)
C
         CALL SETFNT (0)
         CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
         CALL SET3DCOORD_POLAR1 (
     &        0.5*(XMIN+XMAX), 0.5*(YMIN+YMAX), 0.5*(ZMIN+ZMAX),
     &        RADIUS, THETA, REAL (I))
         CALL SETPROJECTIONTYPE (0)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 1)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 1)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 1)
         CALL GVALUE3DX (XMIN, XSCAL, 1, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 1, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 1, 1)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 11)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 11)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 11)
         CALL GVALUE3DX (XMIN, XSCAL, 11, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 11, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 11, 1)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 21)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 21)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 21)
         CALL GVALUE3DX (XMIN, XSCAL, 21, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 21, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 21, 1)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 31)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 31)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 31)
         CALL GVALUE3DX (XMIN, XSCAL, 31, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 31, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 31, 1)      
C     
         CALL GAxis3D (2)
         CALL PAGEE
      ENDDO
C     --------------------------------------------------------------------
      DO I = 30, 360, 45
         CALL PAGES
         CALL SETCHS (0.35,0.0)
         CALL SETFNT (2)
C     
         CALL GTEXTX (22.0, 17.0, "@$#f$#=@", 0)
         CALL INQTSZ ("$#f$#=", 6, SIZE)
         CALL GNUMBI (22.0+SIZE, 17.0, i, 0)
C     
         CALL SETFNT (0)
         CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
         CALL SET3DCOORD_POLAR1 (
     &        0.5*(XMIN+XMAX), 0.5*(YMIN+YMAX), 0.5*(ZMIN+ZMAX),
     &        RADIUS, THETA, REAL (I))
         CALL SETPROJECTIONTYPE (0)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 4)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 4)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 4)
         CALL GVALUE3DX (XMIN, XSCAL, 4, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 4, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 4, 1)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 14)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 14)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 14)
         CALL GVALUE3DX (XMIN, XSCAL, 14, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 14, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 14, 1)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 24)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 24)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 24)
         CALL GVALUE3DX (XMIN, XSCAL, 24, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 24, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 24, 1)
C     
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 34)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 34)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 34)
         CALL GVALUE3DX (XMIN, XSCAL, 34, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 34, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 34, 1)      
C
         CALL GAxis3D (2)
         CALL PAGEE
      ENDDO
C     -----------------------------------------------------------------
      DO I = 30, 360, 45
         CALL PAGES
         CALL SETCHS (0.35,0.0)
         CALL SETFNT (2)
C     
         CALL GTEXTX (22.0, 17.0, "@$#f$#=@", 0)
         CALL INQTSZ ("$#f$#=", 6, SIZE)
         CALL GNUMBI (22.0+SIZE, 17.0, I, 0)
C
         CALL SETFNT (0)
         CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
         CALL SET3DCOORD_POLAR1 (
     &        0.5*(XMIN+XMAX), 0.5*(YMIN+YMAX), 0.5*(ZMIN+ZMAX),
     &        RADIUS, THETA, REAL (I))
         CALL SETPROJECTIONTYPE (0)
C
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 5)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 5)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 5)
         CALL GVALUE3DX (XMIN, XSCAL, 5, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 5, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 5, 1)
C         
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 15)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 15)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 15)
         CALL GVALUE3DX (XMIN, XSCAL, 15, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 15, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 15, 1)
C         
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 25)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 25)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 25)
         CALL GVALUE3DX (XMIN, XSCAL, 25, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 25, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 25, 1)
C         
         CALL GSCALE3DX (XMIN, XSCAL, 0.3, 35)
         CALL GSCALE3DY (YMIN, YSCAL, 0.3, 35)
         CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 35)
         CALL GVALUE3DX (XMIN, XSCAL, 35, 1)
         CALL GVALUE3DY (YMIN, YSCAL, 35, 1)
         CALL GVALUE3DZ (0.0,  ZSCAL, 35, 1)      
C     
         CALL GAxis3D (2)
         CALL PAGEE
      ENDDO
C     -----------------------------------------------------------------
 9999 CALL GSCLOS
      STOP
      END
