const project = new Project('ShaderTest');

project.addFile('Sources/**');
project.addFile('Shaders/**');
project.setDebugDir('Deployment');

resolve(project);
