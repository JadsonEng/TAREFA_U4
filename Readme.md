📋 Jadson de Jesus Santos - Tarefa de Interrupção

📚 Enunciado da Tarefa

Para consolidar a compreensão dos conceitos relacionados ao uso de interrupções no
microcontrolador RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab, propõe-se
a seguinte tarefa prática.

🎯 Objetivos

• Compreender o funcionamento e a aplicação de interrupções em microcontroladores.
• Identificar e corrigir o fenômeno do bouncing em botões por meio de debouncing via software.
• Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812.
• Fixar o estudo do uso de resistores de pull-up internos em botões de acionamento.
• Desenvolver um projeto funcional que combine hardware e software.

📑 Funcionamento do Projeto

• O LED RGB pisca cinco vezes por segundo de forma assíncrona por meio de uma função de repetição;
• Os números são exibidos na matriz de LEDs ws2818b;
• Os botões A e B chamam uma função de interrupção que decrementa ou incrementa o número exibido;
• Acionar o botão A, descrementa o número exibido na matriz de LEDs;
• Acionar o botão B, incrementa o número exibido na matriz de LEDs.

▶️ Link do vídeo: https://youtu.be/cuBatj16R-c