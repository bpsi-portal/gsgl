C
C     SAMPLE PROGRAM sample3d-3.f
C
      DIMENSION GX(101), GY(101), GZ(101)
C
      CALL GSOPEN ! GSGL の初期化
      XL=10.0     ! x軸方向の長さ
      YL=10.0     ! y軸方向の長さ
      ZL=10.0     ! z軸方向の長さ
      PHI=50.0    ! 極座標の φ
      THETA=60.0  ! 極座標の θ
      RADIUS=10.0 ! 極座標の r
C
      N = 101
      DO I = 1, N
         GX(I) = SIN(0.1*REAL(I-1))
         GY(I) = COS(0.1*REAL(I-1))
         GZ(I) = 0.1*REAL(I-1)
      ENDDO
      CALL GMNMX1(GX,1,N,1,XMIN,XMAX) ! GX の最大値，最小値を求める
      CALL GMNMX1(GY,1,N,1,YMIN,YMAX) ! GY の最大値，最小値を求める
      CALL GMNMX1(GZ,1,N,1,ZMIN,ZMAX) ! GZ の最大値，最小値を求める
C     
      CALL GQSCAL(XMIN,XMAX,GXMIN,GXMAX,XSCAL) ! 目盛りの間隔，開始値
      CALL GQSCAL(YMIN,YMAX,GYMIN,GYMAX,YSCAL) ! 目盛りの間隔，開始値
      CALL GQSCAL(ZMIN,ZMAX,GZMIN,GZMAX,ZSCAL) ! 目盛りの間隔，開始値
C     
      CALL PAGES                ! ページの開始
      CALL SETCHS(0.50,0.0)     ! フォントの大きさ 0.50 cm
      CALL SETFNT(2)            ! フォント番号
C     
      CALL GDEFIN3D(XL ,YL ,ZL, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX)
      CALL SET3DCOORD_POLAR1 (
     &     0.5*(XMIN+XMAX), 0.5*(YMIN+YMAX), 0.5*(ZMIN+ZMAX),
     &     RADIUS, THETA, PHI)
C     
      CALL GSCALE3DX (XMIN, XSCAL, 0.3, 0) ! x軸の目盛り
      CALL GSCALE3DY (YMIN, YSCAL, 0.3, 0) ! y軸の目盛り
      CALL GSCALE3DZ (0.0,  ZSCAL, 0.3, 0) ! z軸の目盛り
      CALL GVALUE3DX (XMIN, XSCAL, 2, 1) ! x軸の目盛りの数値
      CALL GVALUE3DY (YMIN, YSCAL, 2, 1) ! y軸の目盛りの数値
      CALL GVALUE3DZ (0.0,  ZSCAL, 2, 1) ! z軸の目盛りの数値
      CALL SetRGB (0.0, 1.0, 1.0) ! 水色に設定
      CALL GPLOTP3D (GX, GY, GZ, 1, 101, 1, 0, 0, 0)
      CALL SetRGB (0.0, 0.0, 0.0) ! 黒色に設定
      Call GAxis3D (2)          ! 座標軸の描画
      Call GDrawBack3D (0.3, 0.3, 0.3) ! 背景を灰色に．
C     
      CALL PAGEE                ! ページの終了
      CALL GSCLOS               ! GSGL の終了
      STOP
      END
