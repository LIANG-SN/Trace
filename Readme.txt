(If there is no instruction below, please simply load the .ray file and click render)


W adaptive termination				yes
There is a threshold slider.


W antialiasing by supersampling			yes
+W jittering					yes/no


B spot lights: soptlight.ray			yes

B background image: testBackground.ray			yes
Load background.bmp in samples and enable background. Please remember to increase the depth.

B accelerate shadow atteuation			yes/no
B overlapping tranparent objects		yes/no

BW texture mapping (diffuse and transparent): earth.ray		yes
Sample texture mapping. Load wordmap.bmp and enable texture map.

BW antialiasing by adaptive supersampling	yes/no
BW Warn model					yes/no

2B bump mapping: map.ray				yes
Load worldmap_normal.bmp and enable bump map. You can also load wordmap.bmp and enable texture map at the same time.


2B solid/procedural texture: earth.ray				yes
Load solid_texture.bmp and enable solid texture.

2B new geometry (quadrics): hyperbolic.ray or paraboloid.ray	yes

2B height fields				yes/no
2B first 					yes/no
1.5B next for each Dof, soft shadows, 		yes/no [corrected 04/16/21]
     motion blur, glossy reflection		yes/no

3B high lever geometry (e.g. metaballs): metaball.ray		yes

3B ray-intersection optimization		yes/no

4B realistic shading model:simpleSamples/box.ray	yes
You can see the different when you enable physical shading.

4B CSG: csg.ray					yes
4B caustics					yes/no

Caustic: caustic.ray
	first select caustic checkbox, then load scene, caustic will be initiallized automatically.
	increase depth.
	unselect the checkbox before load next scene.