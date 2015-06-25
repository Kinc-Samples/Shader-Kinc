var solution = new Solution('ShaderTest');
var project = new Project('ShaderTest');

project.addFile('Sources/**');
project.setDebugDir('Deployment');

project.addSubProject(Solution.createProject('Kore'));

solution.addProject(project);

return solution;
