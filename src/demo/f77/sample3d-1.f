C
C     SAMPLE PROGRAM sample3d-1.f
C
      DIMENSION X(3), Y(3), Z(3)
      CALL GSOPEN ! GSGL の初期化
      XL=10.0     ! x軸の長さ
      YL=10.0     ! y軸の長さ
      ZL=10.0     ! z軸の長さ
      PHI=50.0    ! 極座標の φ
      THETA=60.0  ! 極座標の θ
      RADIUS=10.0 ! 極座標の r
      XMIN=-1.0
      YMIN=-1.0
      ZMIN=-1.0
      XMAX= 1.0
      YMAX= 1.0
      ZMAX= 1.0
C
      CALL PAGES  ! ページの開始
      CALL GDEFIN3D (XL, YL, ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (
     &     0.5*(XMIN+XMAX), 0.5*(YMIN+YMAX), 0.5*(ZMIN+ZMAX),
     &     RADIUS, THETA, PHI)
C
      Call move3D (0.0, 0.0, 0.0)
      Call draw3D (1.0, 0.0, 0.0)
      Call draw3D (1.0, 1.0, 0.0)
      Call draw3D (0.0, 1.0, 0.0)
      Call draw3D (0.0, 0.0, 0.0) ! 正方形

      X(1) = 0.0
      X(2) = 1.0
      X(3) = 1.0
      Y(1) = 0.0
      Y(2) = 1.0
      Y(3) = 0.0
      Z(1) = 1.0
      Z(2) = 1.0
      Z(3) = 1.0
      Call setrgb (0.0, 1.0, 1.0)
      Call poly3D (X, Y, Z, 3) ! 3角形のポリゴン
C
      CALL PAGEE  ! ページの終了
      CALL GSCLOS ! GSGL の終了
      STOP
      END
