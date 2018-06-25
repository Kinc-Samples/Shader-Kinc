let project = new Project('ShaderTest');

project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
