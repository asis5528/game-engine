let position = aspectedPoints
let start = progressTime;
let tail = progressTime-0.5;
            let number = position.count
            var newPos: [simd_float2] = []
            
            for (i, _) in position.enumerated() {
                
                var pos: simd_float2;
				if(floor(tail)==i){
					let factor = tail-i;
					pos = interpolte(position[i],position[i+1],factor);
					newPos.append(pos);
				}
				if(tail<i){
					newPos.append(position[i]);
				}


				if(floor(start)==i){
					let factor = start -i;
					pos = interpolte(position[i],position[i+1],factor);
					newPos.append(pos);
					break;
				}
				

               
               
            }
           