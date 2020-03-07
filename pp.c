#include <lcom/lcf.h>
#include <lcom/pp.h>
#include <stdint.h>
static bool status_error = false;
uint8_t packet;
int mouse_hook_id = 2;

int main(int argc, char *argv[])
{
    lcf_set_language("EN-US");
    lcf_trace_calls("/home/lcom/labs/pp/trace.txt");
    lcf_log_output("/home/lcom/labs/pp/output.txt");
    if (lcf_start(argc, argv)) return 1;
    lcf_cleanup();

    return 0;
}

int(pp_test_packet)(uint32_t cnt)
{
  int ipc_status, r, irq_set = 0;
  message msg;
  bool done = false;
  uint8_t packet_array[3];
  index = 0;

  if(mouse_subscribe_int(&irq_set)) return 1;
  sys_irqdisable(&mouse_hook_id);
  mouse_enable_data_reporting();
  sys_irqenable(&mouse_hook_id);

  // interrupt loop
  while (!done)
  {
    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
       case HARDWARE:
          if (msg.m_notify.interrupts & irq_set)
          {
            mouse_ih();
            if(check_complete_packet(packet_array, index))
            {
              struct packet pp;
              create_parsed_packet(packet_array, &pp);
              mouse_print_packet(&pp);
              index = 0;
              cnt--;
              if(cnt == 0) done = true;
            }
          }
        break;
       default:
        break;
      }
    }
  }

  sys_irqdisable(&mouse_hook_id);
  mouse_disable_data_reporting();
  sys_irqenable(&mouse_hook_id);
  if(mouse_unsubscribe_int(&irq_set)) return 1;

  return r;
}

int(pp_test_optimized)(uint32_t cnt) 
{

}



/*


Têm aqui o ficheiro da prova prática de LCOM do ano passado.
É muito simples para quem anda a fazer as merdas,
mas se andam a ser carregados não é crítico, é chill.
Não tenho enunciado mas lembro-me ainda o que era para fazer
(só é pena não ter como por aqui os testes automáticos, mas wtv)
O 1º exercício era para basicamente repetir a primeira função do rato
(ler packets, dar parse, fazer display), nada de mais.
Por isso arranjem um github fixe de alguém que já tenha feito,
e decorem essa merda, até porque se este não tiver cotação total,
o 2º vale ZERO (Soutos gonna Soutate, fml)

O 2º exercício era para fazer uma versão otimizada do "protocolo" do rato.
No primeiro byte do packet, há dois bits que indicam se há displacement nos eixos X e Y,
sendo que os outros dois bytes correspondem ao valor destes displacements.
No 2º exercício é suposto implementar um protocolo mais eficiente,
em que só são enviados os outros bytes se houver displacement.
Por outras palavras:
 - bit deltaX e bit deltaY com valor 1: três bytes para ler, igual ao costume
 - um dos bits a zero e o outro a 1: dois bytes para ler, sendo que o que tiver o bit a zero não é enviado pelo rato
 - os dois bits a zero: um byte para ler (que é o byte que já foi lido), logo monta-se logo o packet.
Não tenho muito mais info para vos dar mas isto já deve ajudar a passar 
pelo menos 2 ou 3 pessoas que podiam chumbar aqui xd
Also, alguém dê tag nisto para não estar enterrado quando for o teste


====================================================================================================
====================================================================================================
A prova de programação é muito semelhante a um lab, apenas mais curta e mais simples, sendo realizada em Linux nos computadores das salas reservadas. Mais concretamente, têm que implementar apenas 2 funções semelhantes às funções de teste dos diferentes "labs".
A primeira função é bastante simples e vale 60% da nota final. A segunda função vale os restantes 40%, contudo terão que ter mais de 2/3 da cotação na primeira função para que a segunda função seja avaliada: a primeira função avalia o que consideramos essencial para a aprovação a LCOM.
Além disso, a prova é sobre um dispositivo de E/S que usaram num dos labs, mas o controlador é diferente. Assim, antes de começarem a implementação, deverão ler atentamente o enunciado: uma solução para o controlador usado nos labs terá quase de certeza absoluta uma nota próxima de zero, ou mesmo zero. Quanto aos conhecimentos que têm que ter são os que consideramos essenciais em LCOM: nomeadamente, saber testar ou alterar bits dum nº inteiro na linguagem C, ler-de/escrever-em registos do controlador usando as chamadas ao Kernel do Minix, subscrever interrupções de dispositivos de E/S em Minix e receber as notificações respetivas. Como o código para a receção da notificação de interrupções é um pouco mais elaborado, o esqueleto correspondente será fornecido no ficheiro com a função main()