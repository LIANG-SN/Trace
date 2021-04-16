(If there is no instruction below, please simply load the .ray file and click render)


W adaptive termination				yes
There is a threshold slider.


W antialiasing by supersampling			yes
+W jittering					yes


B spot lights: soptlight.ray			yes

B background image: testBackground.ray			yes
Load background.bmp in samples and enable background. Please remember to increase the depth.

B accelerate shadow atteuation			yes
	recursively trace ray towards light to handle trasparent object, accelerated by bounding volumn hierachy

B overlapping tranparent objects: refra_overlapping.ray		yes

BW texture mapping (diffuse and transparent): earth.ray		yes
Sample texture mapping. Load wordmap.bmp and enable texture map.

BW antialiasing by adaptive supersampling	yes

BW Warn model: shape_light.ray					yes

2B bump mapping: map.ray				yes
Load worldmap_normal.bmp and enable bump map. You can also load wordmap.bmp and enable texture map at the same time.


2B solid/procedural texture: earth.ray				yes
Load solid_texture.bmp and enable solid texture.

2B new geometry (quadrics): hyperbolic.ray or paraboloid.ray	yes

2B height fields: blank.ray & hf__512__.bmp				yes
	Enable height field and load height field image(hf__512__.bmp), then load the blank.ray and rander.
	Uncheck height field checkbox before load the next scene, or the field will stay in next scene

2B first soft shadows: softShadow.ray  yes
	enable soft shadow checkbox
 					
1.5B next for Dof: depthOfField.ray	yes
	enable the corresponding checkbox
	Though the focal length is hardcoded to be 1 in the project, the effect can be seen by comparing with disabling DoF

1.5B next for motion blur: threeBall.ray	yes
	enable the corresponding checkbox

1.5B next for glossy reflection: glossyReflection.ray		yes
	enable the corresponding checkbox

3B high lever geometry (e.g. metaballs): metaball.ray		yes

3B ray-intersection optimization		yes
	Optimized by bounding Volumn hierachy
	The best way to test this is load recurse_depth.ray or meteorite.ray in simpleSamples and tune the size to max, 
	then compare the speed with/without optimization.

4B realistic shading model:simpleSamples/box.ray	yes
	You can see the different when you enable physical shading.

4B CSG: csg.ray					yes

4B caustics: caustic.ray			yes

	first select caustic checkbox, then load scene, caustic will be initiallized automatically.
	increase depth.
	unselect the checkbox before load next scene.

4B particle system: meteorite.ray		yes
Though BVH can optimize the speed, it's still slow, prevent the size being too large and be patient pls.
Since it has so many particles, it's a good place to test the effect of BVH.