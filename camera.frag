#version 330 core

in vec3 ourNormal;
in vec3 FragPos;
in vec3 ourColor;
in vec2 TexCoord;
uniform vec3 luzDir;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientI;

//	texture sampler: mapeo
uniform sampler2D texture1;
uniform sampler2D texture2;



void main(){
	
	// ambiente
	//float ambientI = 0.7;
	vec3 ambient = ambientI + lightColor;

	// angulo de 15 grados
	vec3 ld = normalize(-lightPos);
	// hay que cambiar la posicion de la luz por su direccion para que sea direccionable

	ld = luzDir;
	vec3 fd = normalize (vec3 (FragPos-lightPos));

	if(acos(-dot (fd,ld)) < radians(45.5)) {
		// diffuse
		vec3 norm = normalize(ourNormal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm,lightDir), 0.0);
		vec3 diffuse = diff + lightColor;

		// specular
		float specularStrength = 0.1;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir,norm);
		float spec = pow(max(dot(viewDir,reflectDir), 0.0),128);
		vec3 specular = specularStrength * spec * lightColor;
		
		//texturas
		vec4 text = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.3);
		vec3 textureColor = text.rgb;


		// Resultado 
		vec3 result = (ambient + diffuse + specular) * textureColor;

		gl_FragColor = vec4 (result, 1.0f);
	}
	else {
		// Texturas
		vec4 text = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.3); 
		vec3 textureColor = text.rgb;

		vec3 result = (ambient) * lightColor * textureColor;
		gl_FragColor = vec4 (result, 1.0f);
	}
}