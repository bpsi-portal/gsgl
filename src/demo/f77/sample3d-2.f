C
C     SAMPLE PROGRAM sample3d-2.f
C
      CALL GSOPEN ! GSGL の初期化
      XL=10.0     ! x軸方向の長さ
      YL=10.0     ! y軸方向の長さ
      ZL=10.0     ! z軸方向の長さ
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
      CALL GDEFIN3D (XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (
     &     0.5*(XMIN+XMAX), 0.5*(YMIN+YMAX), 0.5*(ZMIN+ZMAX),
     &     RADIUS, THETA, PHI)
C
      Call SetRGB (1.0, 0.0, 0.0) ! 赤色に設定
      Call gsglWireTorus (0.0, 0.0, 0.0, ! トーラスの中心の座標
     &     0.0, 0.0, 1.0,       ! トーラスの軸を決めるベクトル
     &     0.25, 1.0, 20, 20)   ! 小半径，大半径
      CALL PAGEE  ! ページの終了
      CALL GSCLOS ! GSGL の終了
      STOP
      END
