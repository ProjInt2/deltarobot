import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity
{
    id: arrows
    
    property real length: 1.0
    property real radius: 0.06
    
    CylinderMesh
    {
        id: stem
        length: arrows.length
        radius: arrows.radius / 2
        rings: 2
        slices: 8
    }
    
    ConeMesh
    {
        id: coneMesh
        length: arrows.radius*2
        bottomRadius : arrows.radius
        hasBottomEndcap : true
        hasTopEndcap : true
        rings : 2
        slices : 8
        topRadius : arrows.radius * 0.0001
        
    }
    
    // X
    
    PhongMaterial
    {
        id: xMaterial
        ambient: "red"
        diffuse: "red"
        specular: "pink"
    }
    
    Transform
    {
        id: xTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.rotate(-90, Qt.vector3d(0,0,1));
            m.translate(Qt.vector3d(0, length/2, 0));
            return m;
        }
    }
    
    Entity
    {
        id: xStem
        components: [stem, xMaterial, xTransform]
    }
    
    Transform
    {
        id: xConeTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.rotate(-90, Qt.vector3d(0,0,1));
            m.translate(Qt.vector3d(0, length, 0));
            return m;
        }
    }
    
    Entity
    {
        id: xCone
        components: [coneMesh, xMaterial, xConeTransform]
    }
    
    // Y
    
    PhongMaterial
    {
        id: yMaterial
        ambient: "green"
        diffuse: "green"
        specular: "pink"
    }
    
    Transform
    {
        id: yTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(Qt.vector3d(0, length/2, 0));
            return m;
        }
    }
    
    Entity
    {
        id: yStem
        components: [stem, yMaterial, yTransform]
    }
    
    Transform
    {
        id: yConeTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(Qt.vector3d(0, length, 0));
            return m;
        }
    }
    
    Entity
    {
        id: yCone
        components: [coneMesh, yMaterial, yConeTransform]
    }
    
    // Z
    
    PhongMaterial
    {
        id: zMaterial
        ambient: "blue"
        diffuse: "blue"
        specular: "pink"
    }
    
    Transform
    {
        id: zTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.rotate(90, Qt.vector3d(1,0,0));
            m.translate(Qt.vector3d(0, length/2, 0));
            return m;
        }
    }
    
    Entity
    {
        id: zStem
        components: [stem, zMaterial, zTransform]
    }
    
    Transform
    {
        id: zConeTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.rotate(90, Qt.vector3d(1,0,0));
            m.translate(Qt.vector3d(0, length, 0));
            return m;
        }
    }
    
    Entity
    {
        id: zCone
        components: [coneMesh, zMaterial, zConeTransform]
    }
    
}
