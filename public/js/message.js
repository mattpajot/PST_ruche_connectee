var message = {
	props: ['message', 'poids_entier', 'poids_decimal', 'temperature_entier', 'temperature_decimal', 'humidite_entier', 'humidite_decimal', 'date'],
    template: `
    <v-app>
		<v-container>
			<v-row>
				<v-col cols="7">
					<h1>
						{{date}}
					</h1>
				</v-col>
				<v-col>
					<v-img src="images/miel_2.png" width="40" v-if="message.seqNumber %2 == 0"> </v-img>
				</v-col>
			</v-row>
			<v-row>
				<v-col>
		            <v-simple-table>
			            <tbody>
			                <tr>
			                    <td> Poids </td>
			                    <td> {{ poids_entier }}.{{ poids_decimal }} kg </td>
			                </tr>
			                <tr>
			                    <td> Température </td>
			                    <td> {{ temperature_entier }}.{{ temperature_decimal }} °C </td>
			                </tr>
			                <tr>
			                    <td> Humidité </td>
			                    <td> {{ humidite_entier }}.{{ humidite_decimal }} % </td>
			                </tr>
			            </tbody>
			        </v-simple-table>
			    </v-col>
			    <v-col cols="3">
		        	<v-progress-circular
				      :size="100"
				      :width="5"
				      :value="humidite_entier"
				      color="blue"
				    >
				       <v-icon>mdi-water</v-icon>
				      {{ humidite_entier }}.{{ humidite_decimal }} %
				    </v-progress-circular>
		        </v-col>
		        <v-col cols="3">
		        	<v-progress-circular
				      :size="100"
				      :width="5"
				      :value="temperature_entier"
				      color="red"
				    >
				       <v-icon>mdi-thermometer</v-icon>
				      {{ temperature_entier }}.{{ temperature_decimal }} °C
				    </v-progress-circular>
		        </v-col>
		    </v-row>
		    <v-row>
		    	<v-col cols="4">
		    		<v-img src="images/miel_1.png" width=100 v-if="message.seqNumber %2 != 0"> </v-img>
		    	</v-col>
		    	<v-col cols="5">
		    	</v-col>
		    	<v-col>
		    		<v-img src="images/abeille_1.png" width="40" v-if="message.seqNumber %5 == 0"> </v-img>
		    	</v-col>
		    </v-row>
		</v-container>
	</v-app>
`
};