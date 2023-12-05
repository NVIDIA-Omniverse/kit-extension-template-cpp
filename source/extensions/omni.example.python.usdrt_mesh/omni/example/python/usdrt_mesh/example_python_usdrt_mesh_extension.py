## Copyright (c) 2023, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##

import carb
import carb.events
import numpy as np
import omni.ext
import omni.kit.app
import omni.ui
import omni.usd
import usdrt

PLANE_SUBDIV = 32
PLANE_EXTENT = 50
PLANE_HEIGHT = 10
PRIM_PATH = f"/World/Plane{PLANE_SUBDIV}x{PLANE_SUBDIV}"


class ExamplePythonUsdrtMeshExtension(omni.ext.IExt):
    def on_startup(self, ext_id):
        self.sub = None
        self.step = 0
        self.playing = False
        self.init_ui()

    def on_shutdown(self):
        if self.sub:
            self.sub.unsubscribe()
            self.sub = None
        self.step = 0
        self.playing = False

    def init_ui(self):
        def create_mesh():
            stage = get_usdrt_stage()
            create_mesh_usdrt(stage, PRIM_PATH, PLANE_SUBDIV, PLANE_SUBDIV)

        def delete_mesh():
            stage = get_usdrt_stage()
            delete_prim_usdrt(stage, PRIM_PATH)

        def toggle_update_mesh():
            self.playing = not self.playing
            if not self.sub:
                self.init_on_update()

        def toggle_mesh_visibility():
            stage = get_usdrt_stage()
            prim = stage.GetPrimAtPath(PRIM_PATH)
            attr = prim.GetAttribute("_worldVisibility")
            val = attr.Get()
            attr.Set(not val)
            return

        def save_stage_to_file():
            stage = get_usdrt_stage()
            stage.WriteToLayer("example_python_usdrt_mesh_example.usda")
            return

        self.window = omni.ui.Window("omni.example.python.usdrt_mesh", width=300, height=300)
        style = {
            # "color": omni.ui.color.WHITE,
            # "background_color": omni.ui.color.BLACK,
        }
        self.window.frame.style = style
        with self.window.frame:
            with omni.ui.VStack():
                with omni.ui.HStack():
                    omni.ui.Button("Create Plane").set_clicked_fn(create_mesh)
                    omni.ui.Button("Delete Plane").set_clicked_fn(delete_mesh)
                with omni.ui.HStack():
                    omni.ui.Button("Toggle Update").set_clicked_fn(toggle_update_mesh)
                    omni.ui.Button("Toggle Visibility").set_clicked_fn(toggle_mesh_visibility)
                    omni.ui.Button("Save").set_clicked_fn(save_stage_to_file)

    def init_on_update(self):
        @carb.profiler.profile(zone_name="omni.example.python.usdrt_mesh.on_update")
        def on_update(e: carb.events.IEvent):
            if not self.playing:
                return
            try:
                stage = get_usdrt_stage()
                self.step += 1
                update_mesh_usdrt(stage, PRIM_PATH, PLANE_SUBDIV, PLANE_SUBDIV, self.step)
            except Exception as e:
                carb.log_error(e)
            return

        update_stream = omni.kit.app.get_app().get_update_event_stream()
        self.sub = update_stream.create_subscription_to_pop(on_update, name="omni.example.python.usdrt_mesh.on_update")
        return


def get_usdrt_stage() -> usdrt.Usd.Stage:
    ctx = omni.usd.get_context()
    stage = usdrt.Usd.Stage.Attach(ctx.get_stage_id())
    return stage


def create_mesh_usdrt(stage: usdrt.Usd.Stage, prim_path: str, num_x_divisions: int, num_z_divisions: int):
    mesh = usdrt.UsdGeom.Mesh.Define(stage, prim_path)

    # Create the vertices and face counts
    vertices = calculate_mesh_vertices(num_x_divisions, num_z_divisions, 0)
    face_vertex_counts = []
    face_vertex_indices = []

    for z in range(num_z_divisions):
        for x in range(num_x_divisions):
            vertex0 = z * (num_x_divisions + 1) + x
            vertex1 = vertex0 + 1
            vertex2 = (z + 1) * (num_x_divisions + 1) + x
            vertex3 = vertex2 + 1
            face_vertex_counts.append(4)
            face_vertex_indices.extend([vertex0, vertex1, vertex3, vertex2])

    # Set the mesh data
    mesh.CreatePointsAttr().Set(usdrt.Vt.Vec3fArray(vertices))
    mesh.CreateFaceVertexCountsAttr().Set(usdrt.Vt.IntArray(face_vertex_counts))
    mesh.CreateFaceVertexIndicesAttr().Set(usdrt.Vt.IntArray(face_vertex_indices))

    prim = mesh.GetPrim()
    # Visibility Attribute
    attr = prim.CreateAttribute("_worldVisibility", usdrt.Sdf.ValueTypeNames.Bool, True)
    attr.Set(True)

    # Set the xform
    xformable = usdrt.Rt.Xformable(prim)
    xformable.CreateWorldPositionAttr(usdrt.Gf.Vec3d(0.0, 0.0, 0.0))
    xformable.CreateWorldScaleAttr(usdrt.Gf.Vec3f(1.0, 1.0, 1.0))
    xformable.CreateWorldOrientationAttr(usdrt.Gf.Quatf(0.0, 0.0, 0.0, 1.0))

    # Set the extents
    bound = usdrt.Rt.Boundable(prim)
    world_ext = bound.CreateWorldExtentAttr()
    world_ext.Set(
        usdrt.Gf.Range3d(
            usdrt.Gf.Vec3d(-PLANE_EXTENT, -PLANE_EXTENT, -PLANE_EXTENT),
            usdrt.Gf.Vec3d(PLANE_EXTENT, PLANE_EXTENT, PLANE_EXTENT),
        )
    )
    return mesh


def delete_prim_usdrt(stage: usdrt.Usd.Stage, prim_path: str):
    stage.RemovePrim(prim_path)
    return


def update_mesh_usdrt(stage: usdrt.Usd.Stage, prim_path: str, num_x_divisions: int, num_z_divisions: int, step: int):
    # Find the prim
    prim = stage.GetPrimAtPath(prim_path)
    if not prim.IsValid():
        carb.log_verbose(f"Prim at '{prim_path}' is invalid")
        return

    vertices = calculate_mesh_vertices(num_x_divisions, num_z_divisions, step)

    # Set the mesh data
    mesh = usdrt.UsdGeom.Mesh(prim)
    mesh.CreateVisibilityAttr().Set(True)
    mesh.GetPointsAttr().Set(usdrt.Vt.Vec3fArray(vertices))
    return mesh


def calculate_mesh_vertices(num_x_divisions: int, num_z_divisions: int, step: int) -> [float]:
    x_positions = np.linspace(-PLANE_EXTENT, PLANE_EXTENT, num_x_divisions + 1)
    z_positions = np.linspace(-PLANE_EXTENT, PLANE_EXTENT, num_z_divisions + 1)
    x_grid, z_grid = np.meshgrid(x_positions, z_positions)

    tau = 6.28318
    s = 100.0
    t = step / s
    sx = tau / s
    sz = tau / s
    y_grid = PLANE_HEIGHT * (np.cos(sx * x_grid + t) + np.sin(sz * z_grid + t))

    vertices = np.column_stack((x_grid.flatten(), y_grid.flatten(), z_grid.flatten()))
    return vertices.tolist()
