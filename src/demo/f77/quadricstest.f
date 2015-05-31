C
C     SAMPLE PROGRAM
C
      include '../../gsglf.h'
      DIMENSION VX(3), VY(3), VZ(3)
      EXTERNAL R2G2B,BLACK,WHITE,R2G2B_17
C
      IXMAX=31
      IYMAX=31
      CALL GSOPEN
C
      XL=10.0
      YL=10.0
      ZL=10.0
C
      PHI=50.0
      THETA=60.0
      RADIUS=50.0
C      
      XMIN=0.0
      YMIN=0.0
      ZMIN=0.0
      XMAX=2.0
      YMAX=2.0
      ZMAX=2.0
C
 2    WRITE (6,*) 'INPUT PHI,THETA'
      READ (5,*,END=9999,ERR=2) PHI, THETA
C     -------------------------------------------------------------
      CALL PAGES
C
      CALL gsglEnableLighting
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (
     &     0.5*(XMIN+XMAX),0.5*(YMIN+YMAX),0.5*(ZMIN+ZMAX),
     &     RADIUS, THETA, PHI)
      CALL SETPROJECTIONTYPE (GSGL_PROJECTION_TYPE_ORTHOGONAL)
C
      CALL gsglProjImgScale (0.75, 0.75)
      CALL gsglProjImgTranslate (-0.125*GSGL_PAPER_WIDTH,
     &                           -0.125*GSGL_PAPER_HEIGHT) 
      CALL move (0.0, 0.0)
      CALL draw (GSGL_PAPER_WIDTH, 0.0)
      CALL draw (GSGL_PAPER_WIDTH, GSGL_PAPER_HEIGHT)
      CALL draw (0.0, GSGL_PAPER_HEIGHT)
      CALL draw (0.0, 0.0)
      CALL SETCLP (0.0, GSGL_PAPER_WIDTH, 0.0, GSGL_PAPER_HEIGHT)
      CALL SetRGB(1.0, 0.0, 0.0)
      CALL gsglSolidSphere (1.0, 1.0, 1.0, 0.0, 0.0, 1.0,
     &                      0.5, 20, 20)
      CALL SetRGB (0.2, 0.2, 1.0)
      CALL gsglSolidTorus (1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
     &                     0.5, 2.0, 20, 20)
      CALL gsglProjImgIdentity ()
      CALL OFFCLP
C     -------------------------------------------------------------
      CALL gsglProjImgScale (0.4, 0.4)
      CALL gsglProjImgTranslate (0.3*GSGL_PAPER_WIDTH, 
     &                           0.3*GSGL_PAPER_HEIGHT)
      CALL SetRGB(-1.0, 0.0, 0.0)
      CALL move (0.0, 0.0)
      CALL draw (GSGL_PAPER_WIDTH, 0.0)
      CALL draw (GSGL_PAPER_WIDTH, GSGL_PAPER_HEIGHT)
      CALL draw (0.0, GSGL_PAPER_HEIGHT)
      CALL draw (0.0, 0.0)
      CALL SETCLP (0.0, GSGL_PAPER_WIDTH, 0.0, GSGL_PAPER_HEIGHT)
      CALL SetRGB(1.0, 0.0, 0.0)
      CALL gsglWireSphere(1.0, 1.0, 1.0,
     &                    0.0, 0.0, 1.0,
     &                    0.5, 20, 20)
      CALL SetRGB(0.2, 0.2, 1.0)
      CALL gsglWireTorus(1.0, 1.0, 1.0,
     &                   1.0, 0.0, 1.0,
     &                   0.5, 2.0, 20, 20)
      CALL PAGEE
C     -------------------------------------------------------------
      CALL PAGES
C
      CALL gsglEnableLighting
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      VX(1) = 1.0
      VX(2) = 0.0
      VX(3) = 0.0
      VY(1) = 0.0
      VY(2) = 1.0
      VY(3) = 0.0
      VZ(1) = 0.0
      VZ(2) = 0.0
      VZ(3) = 1.0
      CALL SET3DCOORD_CAUT1 (
     &     0.5*(XMIN+XMAX),0.5*(YMIN+YMAX),0.5*(ZMIN+ZMAX),
     &     0.0, VX, VY, VZ)
C
C      CALL gsglProjImgTranslate (XL, 0.0)
C      CALL gsglProjImgScale (3.0, 3.0)
C
      CALL SetRGB (0.6, 0.6, 1.0)
      CALL gsglSolidTorus (1.0, 1.0, 1.0, 0.0, 1.0, 0.0,
     &                     0.5, 2.0, 40, 40)
      CALL SetRGB (0.4, 0.4, 1.0)
      CALL gsglSolidTorus (1.0, 1.0, 1.0, 0.0, 1.0, 0.0,
     &                     0.3, 2.0, 40, 40)
      CALL SetRGB (0.2, 0.2, 1.0)
      CALL gsglSolidTorus (1.0, 1.0, 1.0, 0.0, 1.0, 0.0,
     &                     0.1, 2.0, 40, 40)
C      
      CALL PAGEE
 9999 CALL GSCLOS
      STOP
      END
